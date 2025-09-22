// LDM Download Manager - Background Script (Manifest V3 Service Worker)

class LDMDownloadManager {
  constructor() {
    this.port = null;
    this.pendingDownloads = new Map();
    this.setupEventListeners();
    this.connectToNativeHost();
  }

  setupEventListeners() {
    // Listen for download requests from content scripts
    chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
      this.handleMessage(message, sender, sendResponse);
      return true; // Keep message channel open for async response
    });

    // Listen for context menu clicks
    chrome.contextMenus.onClicked.addListener((info, tab) => {
      this.handleContextMenuClick(info, tab);
    });

    // Create context menu
    this.createContextMenu();

    // Handle download events
    chrome.downloads.onCreated.addListener((downloadItem) => {
      this.handleDownloadCreated(downloadItem);
    });

    chrome.downloads.onChanged.addListener((downloadDelta) => {
      this.handleDownloadChanged(downloadDelta);
    });
  }

  createContextMenu() {
    chrome.contextMenus.create({
      id: "ldm-download-link",
      title: "Download with LDM",
      contexts: ["link"],
    });

    chrome.contextMenus.create({
      id: "ldm-download-image",
      title: "Download Image with LDM",
      contexts: ["image"],
    });

    chrome.contextMenus.create({
      id: "ldm-download-video",
      title: "Download Video with LDM",
      contexts: ["video"],
    });
  }

  connectToNativeHost() {
    try {
      this.port = chrome.runtime.connectNative("com.ldm.downloadmanager");
      this.port.onMessage.addListener((message) => {
        this.handleNativeMessage(message);
      });
      this.port.onDisconnect.addListener(() => {
        console.log("Disconnected from native host");
        this.port = null;
        // Retry connection after delay
        setTimeout(() => this.connectToNativeHost(), 5000);
      });
    } catch (error) {
      console.error("Failed to connect to native host:", error);
    }
  }

  handleMessage(message, sender, sendResponse) {
    switch (message.type) {
      case "INTERCEPT_DOWNLOAD":
        this.interceptDownload(message.data, sendResponse);
        break;
      case "GET_DOWNLOAD_STATUS":
        this.getDownloadStatus(message.downloadId, sendResponse);
        break;
      case "CANCEL_DOWNLOAD":
        this.cancelDownload(message.downloadId, sendResponse);
        break;
      default:
        sendResponse({ success: false, error: "Unknown message type" });
    }
  }

  handleContextMenuClick(info, tab) {
    let url = "";
    let filename = "";

    if (info.linkUrl) {
      url = info.linkUrl;
      filename = this.extractFilenameFromUrl(url);
    } else if (info.srcUrl) {
      url = info.srcUrl;
      filename = this.extractFilenameFromUrl(url);
    }

    if (url) {
      this.startDownload(url, filename, tab.id);
    }
  }

  handleDownloadCreated(downloadItem) {
    // Check if this download should be intercepted
    if (this.shouldInterceptDownload(downloadItem)) {
      chrome.downloads.pause(downloadItem.id);
      this.pendingDownloads.set(downloadItem.id, downloadItem);
      this.sendToNativeHost({
        type: "NEW_DOWNLOAD",
        data: {
          id: downloadItem.id,
          url: downloadItem.url,
          filename: downloadItem.filename,
          referrer: downloadItem.referrer,
          totalBytes: downloadItem.totalBytes,
        },
      });
    }
  }

  handleDownloadChanged(downloadDelta) {
    if (downloadDelta.state && downloadDelta.state.current === "complete") {
      this.pendingDownloads.delete(downloadDelta.id);
    }
  }

  interceptDownload(data, sendResponse) {
    this.startDownload(data.url, data.filename, data.tabId);
    sendResponse({ success: true });
  }

  startDownload(url, filename, tabId) {
    // Send download request to native host
    this.sendToNativeHost({
      type: "START_DOWNLOAD",
      data: {
        url: url,
        filename: filename,
        referrer: tabId ? "" : "", // Could get referrer from tab
        userAgent: navigator.userAgent,
      },
    });
  }

  getDownloadStatus(downloadId, sendResponse) {
    // Query native host for status
    this.sendToNativeHost(
      {
        type: "GET_STATUS",
        data: { downloadId: downloadId },
      },
      (response) => {
        sendResponse(response);
      },
    );
  }

  cancelDownload(downloadId, sendResponse) {
    this.sendToNativeHost(
      {
        type: "CANCEL_DOWNLOAD",
        data: { downloadId: downloadId },
      },
      (response) => {
        sendResponse(response);
      },
    );
  }

  handleNativeMessage(message) {
    switch (message.type) {
      case "DOWNLOAD_STARTED":
        this.handleDownloadStarted(message.data);
        break;
      case "DOWNLOAD_PROGRESS":
        this.handleDownloadProgress(message.data);
        break;
      case "DOWNLOAD_COMPLETED":
        this.handleDownloadCompleted(message.data);
        break;
      case "DOWNLOAD_FAILED":
        this.handleDownloadFailed(message.data);
        break;
      default:
        console.log("Unknown native message type:", message.type);
    }
  }

  handleDownloadStarted(data) {
    // Cancel the browser download if we intercepted it
    if (this.pendingDownloads.has(data.browserDownloadId)) {
      chrome.downloads.cancel(data.browserDownloadId);
      this.pendingDownloads.delete(data.browserDownloadId);
    }

    // Notify content script
    chrome.tabs.sendMessage(data.tabId, {
      type: "DOWNLOAD_STARTED",
      data: data,
    });
  }

  handleDownloadProgress(data) {
    chrome.tabs.sendMessage(data.tabId, {
      type: "DOWNLOAD_PROGRESS",
      data: data,
    });
  }

  handleDownloadCompleted(data) {
    chrome.tabs.sendMessage(data.tabId, {
      type: "DOWNLOAD_COMPLETED",
      data: data,
    });

    // Show notification
    chrome.notifications.create({
      type: "basic",
      iconUrl: "icons/icon128.png",
      title: "Download Completed",
      message: data.filename,
    });
  }

  handleDownloadFailed(data) {
    chrome.tabs.sendMessage(data.tabId, {
      type: "DOWNLOAD_FAILED",
      data: data,
    });
  }

  sendToNativeHost(message, callback) {
    if (this.port) {
      this.port.postMessage(message);
      if (callback) {
        // For responses, we need to handle them in handleNativeMessage
        // This is a simplified version
      }
    } else {
      console.error("No connection to native host");
      if (callback) {
        callback({ success: false, error: "No native connection" });
      }
    }
  }

  shouldInterceptDownload(downloadItem) {
    // Check if LDM should handle this download
    // For now, intercept all downloads
    return true;
  }

  extractFilenameFromUrl(url) {
    try {
      const urlObj = new URL(url);
      const pathname = urlObj.pathname;
      const filename = pathname.substring(pathname.lastIndexOf("/") + 1);
      return filename || "download";
    } catch (e) {
      return "download";
    }
  }
}

// Initialize the download manager
const ldmManager = new LDMDownloadManager();
