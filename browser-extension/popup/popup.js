// LDM Download Manager - Popup Script

class LDMPopup {
    constructor() {
        this.downloads = new Map();
        this.init();
    }

    init() {
        this.bindEvents();
        this.loadDownloads();
        this.checkConnection();
    }

    bindEvents() {
        const downloadBtn = document.getElementById('download-btn');
        const urlInput = document.getElementById('url-input');
        const settingsBtn = document.getElementById('settings-btn');
        const historyBtn = document.getElementById('history-btn');

        downloadBtn.addEventListener('click', () => this.handleDownload());
        urlInput.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                this.handleDownload();
            }
        });
        settingsBtn.addEventListener('click', () => this.openSettings());
        historyBtn.addEventListener('click', () => this.openHistory());

        // Listen for messages from background script
        chrome.runtime.onMessage.addListener((message) => {
            this.handleMessage(message);
        });
    }

    handleDownload() {
        const urlInput = document.getElementById('url-input');
        const url = urlInput.value.trim();

        if (!url) {
            this.showNotification('Please enter a valid URL', 'error');
            return;
        }

        // Send download request to background script
        chrome.runtime.sendMessage({
            type: 'INTERCEPT_DOWNLOAD',
            data: {
                url: url,
                filename: this.extractFilename(url),
                tabId: null // Will be set by background script
            }
        }, (response) => {
            if (response && response.success) {
                this.showNotification('Download started', 'success');
                urlInput.value = '';
            } else {
                this.showNotification('Failed to start download', 'error');
            }
        });
    }

    handleMessage(message) {
        switch (message.type) {
            case 'DOWNLOAD_STARTED':
                this.onDownloadStarted(message.data);
                break;
            case 'DOWNLOAD_PROGRESS':
                this.onDownloadProgress(message.data);
                break;
            case 'DOWNLOAD_COMPLETED':
                this.onDownloadCompleted(message.data);
                break;
            case 'DOWNLOAD_FAILED':
                this.onDownloadFailed(message.data);
                break;
        }
    }

    onDownloadStarted(data) {
        const downloadItem = {
            id: data.id,
            filename: data.filename,
            progress: 0,
            status: 'Starting...',
            speed: 0
        };
        this.downloads.set(data.id, downloadItem);
        this.renderDownloads();
    }

    onDownloadProgress(data) {
        const download = this.downloads.get(data.id);
        if (download) {
            download.progress = data.progress;
            download.speed = data.speed;
            download.status = `${Math.round(data.progress)}%`;
            this.updateDownloadItem(data.id);
        }
    }

    onDownloadCompleted(data) {
        const download = this.downloads.get(data.id);
        if (download) {
            download.progress = 100;
            download.status = 'Completed';
            this.updateDownloadItem(data.id);
            setTimeout(() => {
                this.downloads.delete(data.id);
                this.renderDownloads();
            }, 3000);
        }
    }

    onDownloadFailed(data) {
        const download = this.downloads.get(data.id);
        if (download) {
            download.status = 'Failed';
            this.updateDownloadItem(data.id);
            setTimeout(() => {
                this.downloads.delete(data.id);
                this.renderDownloads();
            }, 5000);
        }
    }

    renderDownloads() {
        const container = document.getElementById('downloads-container');
        container.innerHTML = '';

        for (const [id, download] of this.downloads) {
            const item = this.createDownloadItemElement(download);
            container.appendChild(item);
        }
    }

    createDownloadItemElement(download) {
        const item = document.createElement('div');
        item.className = 'download-item';
        item.dataset.id = download.id;

        item.innerHTML = `
            <div class="filename">${download.filename}</div>
            <div class="progress">
                <div class="progress-bar" style="width: ${download.progress}%"></div>
            </div>
            <div class="status">${download.status}</div>
        `;

        return item;
    }

    updateDownloadItem(id) {
        const download = this.downloads.get(id);
        if (!download) return;

        const item = document.querySelector(`[data-id="${id}"]`);
        if (item) {
            const progressBar = item.querySelector('.progress-bar');
            const status = item.querySelector('.status');

            if (progressBar) {
                progressBar.style.width = `${download.progress}%`;
            }
            if (status) {
                status.textContent = download.status;
            }
        }
    }

    loadDownloads() {
        // Load active downloads from background script
        chrome.runtime.sendMessage({ type: 'GET_ACTIVE_DOWNLOADS' }, (response) => {
            if (response && response.downloads) {
                response.downloads.forEach(download => {
                    this.downloads.set(download.id, download);
                });
                this.renderDownloads();
            }
        });
    }

    checkConnection() {
        // Check connection to native host
        const statusElement = document.getElementById('connection-status');
        chrome.runtime.sendMessage({ type: 'CHECK_CONNECTION' }, (response) => {
            if (response && response.connected) {
                statusElement.textContent = 'Connected';
                statusElement.style.color = 'green';
            } else {
                statusElement.textContent = 'Disconnected';
                statusElement.style.color = 'red';
            }
        });
    }

    openSettings() {
        chrome.tabs.create({ url: chrome.runtime.getURL('options/options.html') });
        window.close();
    }

    openHistory() {
        // Open history page or send message to background
        chrome.runtime.sendMessage({ type: 'OPEN_HISTORY' });
        window.close();
    }

    showNotification(message, type) {
        // Simple notification - could be enhanced
        const notification = document.createElement('div');
        notification.textContent = message;
        notification.style.cssText = `
            position: fixed;
            top: 10px;
            right: 10px;
            background: ${type === 'error' ? '#f44336' : '#4caf50'};
            color: white;
            padding: 10px;
            border-radius: 4px;
            z-index: 1000;
        `;
        document.body.appendChild(notification);
        setTimeout(() => {
            document.body.removeChild(notification);
        }, 3000);
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
}

// Initialize popup when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    new LDMPopup();
});
