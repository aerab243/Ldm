// LDM Download Manager - Content Script

class LDMContentScript {
    constructor() {
        this.init();
    }

    init() {
        this.bindEvents();
        this.injectDownloadButtons();
        this.observeDynamicContent();
    }

    bindEvents() {
        // Listen for messages from background script
        chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
            this.handleMessage(message, sender, sendResponse);
        });

        // Intercept clicks on download links
        document.addEventListener('click', (e) => {
            this.handleClick(e);
        }, true);

        // Intercept form submissions that might be downloads
        document.addEventListener('submit', (e) => {
            this.handleFormSubmit(e);
        });
    }

    handleMessage(message, sender, sendResponse) {
        switch (message.type) {
            case 'DOWNLOAD_STARTED':
                this.showDownloadNotification(message.data, 'started');
                break;
            case 'DOWNLOAD_PROGRESS':
                this.updateDownloadProgress(message.data);
                break;
            case 'DOWNLOAD_COMPLETED':
                this.showDownloadNotification(message.data, 'completed');
                break;
            case 'DOWNLOAD_FAILED':
                this.showDownloadNotification(message.data, 'failed');
                break;
        }
    }

    handleClick(e) {
        const target = e.target;

        // Check if it's a download link
        if (target.tagName === 'A' && (target.download || this.isDownloadLink(target))) {
            e.preventDefault();
            this.interceptDownload(target.href, target.download || this.extractFilename(target.href));
        }

        // Check if it's an injected download button
        if (target.classList.contains('ldm-download-btn')) {
            e.preventDefault();
            const url = target.dataset.url;
            const filename = target.dataset.filename;
            this.interceptDownload(url, filename);
        }
    }

    handleFormSubmit(e) {
        const form = e.target;
        const action = form.action;
        const method = form.method.toUpperCase();

        // Check if form submission might result in a download
        if (method === 'POST' && this.isDownloadForm(form)) {
            e.preventDefault();
            this.interceptFormDownload(form);
        }
    }

    injectDownloadButtons() {
        // Inject download buttons next to media elements
        const mediaElements = document.querySelectorAll('video, audio, img[download]');

        mediaElements.forEach(element => {
            if (!element.nextElementSibling || !element.nextElementSibling.classList.contains('ldm-download-btn')) {
                const downloadBtn = this.createDownloadButton(element);
                element.parentNode.insertBefore(downloadBtn, element.nextSibling);
            }
        });
    }

    createDownloadButton(element) {
        const btn = document.createElement('button');
        btn.className = 'ldm-download-btn';
        btn.textContent = 'Download with LDM';
        btn.style.cssText = `
            margin-left: 10px;
            padding: 5px 10px;
            background-color: #2196F3;
            color: white;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 12px;
        `;

        let url, filename;
        if (element.tagName === 'VIDEO' || element.tagName === 'AUDIO') {
            url = element.src;
            filename = this.extractFilename(url);
        } else if (element.tagName === 'IMG') {
            url = element.src;
            filename = this.extractFilename(url);
        }

        btn.dataset.url = url;
        btn.dataset.filename = filename;

        return btn;
    }

    observeDynamicContent() {
        // Observe for dynamically added content
        const observer = new MutationObserver((mutations) => {
            mutations.forEach((mutation) => {
                if (mutation.type === 'childList') {
                    this.injectDownloadButtons();
                }
            });
        });

        observer.observe(document.body, {
            childList: true,
            subtree: true
        });
    }

    interceptDownload(url, filename) {
        chrome.runtime.sendMessage({
            type: 'INTERCEPT_DOWNLOAD',
            data: {
                url: url,
                filename: filename,
                referrer: window.location.href,
                tabId: null
            }
        });
    }

    interceptFormDownload(form) {
        const formData = new FormData(form);
        const params = new URLSearchParams();

        for (let [key, value] of formData.entries()) {
            params.append(key, value);
        }

        const url = form.action + '?' + params.toString();

        this.interceptDownload(url, 'download');
    }

    isDownloadLink(link) {
        const href = link.href;
        const downloadAttrs = ['download', 'blob:', 'data:'];

        return downloadAttrs.some(attr => href.includes(attr)) ||
               this.hasDownloadExtension(href);
    }

    isDownloadForm(form) {
        // Check if form likely results in a download
        const inputs = form.querySelectorAll('input[type="file"]');
        return inputs.length > 0;
    }

    hasDownloadExtension(url) {
        const downloadExtensions = ['.zip', '.rar', '.7z', '.tar.gz', '.pdf', '.doc', '.docx', '.xls', '.xlsx', '.ppt', '.pptx', '.mp3', '.mp4', '.avi', '.mkv', '.jpg', '.png', '.gif'];
        return downloadExtensions.some(ext => url.toLowerCase().endsWith(ext));
    }

    extractFilename(url) {
        try {
            const urlObj = new URL(url);
            const pathname = urlObj.pathname;
            const filename = pathname.substring(pathname.lastIndexOf('/') + 1);
            return filename || 'download';
        } catch (e) {
            return 'download';
        }
    }

    showDownloadNotification(data, status) {
        const notification = document.createElement('div');
        notification.className = 'ldm-notification';

        let message, bgColor;
        switch (status) {
            case 'started':
                message = `Download started: ${data.filename}`;
                bgColor = '#2196F3';
                break;
            case 'completed':
                message = `Download completed: ${data.filename}`;
                bgColor = '#4CAF50';
                break;
            case 'failed':
                message = `Download failed: ${data.filename}`;
                bgColor = '#F44336';
                break;
        }

        notification.textContent = message;
        notification.style.cssText = `
            position: fixed;
            top: 20px;
            right: 20px;
            background-color: ${bgColor};
            color: white;
            padding: 10px 15px;
            border-radius: 4px;
            z-index: 10000;
            font-family: Arial, sans-serif;
            font-size: 14px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.3);
            max-width: 300px;
        `;

        document.body.appendChild(notification);

        setTimeout(() => {
            if (notification.parentNode) {
                notification.parentNode.removeChild(notification);
            }
        }, 5000);
    }

    updateDownloadProgress(data) {
        // Update any visible progress indicators
        // This could be enhanced to show progress overlays
    }
}

// Initialize content script
const ldmContentScript = new LDMContentScript();
