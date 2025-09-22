# LDM REST API Documentation

## Overview

The LDM (Like Download Manager) REST API provides programmatic access to download management functionality. The API allows third-party applications to integrate with LDM for creating, monitoring, and managing downloads.

**Base URL**: `http://localhost:8080/api/v1`

**Authentication**: Currently not required (future versions may implement API keys)

**Content Type**: `application/json`

## Downloads

### List Downloads

Retrieve a list of downloads with optional filtering.

```http
GET /api/v1/downloads
```

**Query Parameters:**
- `status` (optional): Filter by status (`queued`, `downloading`, `paused`, `completed`, `failed`, `cancelled`)
- `category` (optional): Filter by category ID
- `limit` (optional): Maximum number of results (default: 50)
- `offset` (optional): Number of results to skip (default: 0)

**Response:**
```json
{
  "downloads": [
    {
      "id": 1,
      "url": "https://example.com/file.zip",
      "filename": "file.zip",
      "filepath": "/downloads/file.zip",
      "status": "completed",
      "progress": 1.0,
      "total_size": 1048576,
      "downloaded_size": 1048576,
      "speed": 0,
      "eta": 0,
      "created_at": "2025-01-15T10:30:00Z",
      "category_id": 1,
      "priority": 1
    }
  ],
  "total": 1,
  "limit": 50,
  "offset": 0
}
```

### Create Download

Add a new download to the queue.

```http
POST /api/v1/downloads
```

**Request Body:**
```json
{
  "url": "https://example.com/file.zip",
  "filename": "custom-name.zip",
  "category_id": 1,
  "priority": 1,
  "segments": 4
}
```

**Response:**
```json
{
  "id": 2,
  "url": "https://example.com/file.zip",
  "filename": "custom-name.zip",
  "filepath": null,
  "status": "queued",
  "progress": 0.0,
  "total_size": null,
  "downloaded_size": 0,
  "speed": 0,
  "eta": 0,
  "created_at": "2025-01-15T10:35:00Z",
  "category_id": 1,
  "priority": 1
}
```

### Get Download Details

Retrieve detailed information about a specific download.

```http
GET /api/v1/downloads/{id}
```

**Response:**
```json
{
  "id": 1,
  "url": "https://example.com/file.zip",
  "filename": "file.zip",
  "filepath": "/downloads/file.zip",
  "status": "downloading",
  "progress": 0.75,
  "total_size": 1048576,
  "downloaded_size": 786432,
  "speed": 512000,
  "eta": 450,
  "created_at": "2025-01-15T10:30:00Z",
  "category_id": 1,
  "priority": 1
}
```

### Update Download

Modify download properties.

```http
PUT /api/v1/downloads/{id}
```

**Request Body:**
```json
{
  "status": "paused",
  "priority": 2,
  "category_id": 2
}
```

**Response:**
```json
{
  "id": 1,
  "url": "https://example.com/file.zip",
  "filename": "file.zip",
  "filepath": "/downloads/file.zip",
  "status": "paused",
  "progress": 0.75,
  "total_size": 1048576,
  "downloaded_size": 786432,
  "speed": 0,
  "eta": 0,
  "created_at": "2025-01-15T10:30:00Z",
  "category_id": 2,
  "priority": 2
}
```

### Delete Download

Remove a download from the system.

```http
DELETE /api/v1/downloads/{id}
```

**Response:** 204 No Content

### Pause Download

Pause an active download.

```http
POST /api/v1/downloads/{id}/pause
```

**Response:** 200 OK

### Resume Download

Resume a paused download.

```http
POST /api/v1/downloads/{id}/resume
```

**Response:** 200 OK

## Categories

### List Categories

Retrieve all download categories.

```http
GET /api/v1/categories
```

**Response:**
```json
[
  {
    "id": 1,
    "name": "Documents",
    "description": "PDF files and documents",
    "default_path": "/downloads/documents",
    "color": "#2196F3",
    "icon": "document"
  }
]
```

### Create Category

Create a new download category.

```http
POST /api/v1/categories
```

**Request Body:**
```json
{
  "name": "Videos",
  "description": "Video files",
  "default_path": "/downloads/videos"
}
```

**Response:**
```json
{
  "id": 2,
  "name": "Videos",
  "description": "Video files",
  "default_path": "/downloads/videos",
  "color": null,
  "icon": null
}
```

### Get Category Details

Retrieve information about a specific category.

```http
GET /api/v1/categories/{id}
```

**Response:**
```json
{
  "id": 1,
  "name": "Documents",
  "description": "PDF files and documents",
  "default_path": "/downloads/documents",
  "color": "#2196F3",
  "icon": "document"
}
```

### Update Category

Modify category properties.

```http
PUT /api/v1/categories/{id}
```

**Request Body:**
```json
{
  "name": "Documents & PDFs",
  "description": "All document files",
  "default_path": "/downloads/docs"
}
```

### Delete Category

Remove a category (downloads in this category will be moved to default).

```http
DELETE /api/v1/categories/{id}
```

**Response:** 204 No Content

## History

### Get Download History

Retrieve completed download history.

```http
GET /api/v1/history
```

**Query Parameters:**
- `limit` (optional): Maximum number of results (default: 100)
- `offset` (optional): Number of results to skip (default: 0)
- `success` (optional): Filter by success status

**Response:**
```json
{
  "history": [
    {
      "id": 1,
      "url": "https://example.com/file.zip",
      "filename": "file.zip",
      "size": 1048576,
      "completed_at": "2025-01-15T10:45:00Z",
      "duration": 150,
      "average_speed": 6990,
      "success": true
    }
  ],
  "total": 1
}
```

## Statistics

### Get Statistics

Retrieve download statistics.

```http
GET /api/v1/statistics
```

**Response:**
```json
{
  "total_downloads": 150,
  "total_size": 1073741824,
  "active_downloads": 3,
  "completed_today": 12,
  "average_speed": 2048000
}
```

## Settings

### Get Settings

Retrieve application settings.

```http
GET /api/v1/settings
```

**Query Parameters:**
- `category` (optional): Filter by settings category

**Response:**
```json
{
  "max_concurrent_downloads": "5",
  "default_download_path": "/downloads",
  "enable_notifications": "true",
  "theme": "dark"
}
```

### Update Settings

Modify application settings.

```http
PUT /api/v1/settings
```

**Request Body:**
```json
{
  "max_concurrent_downloads": "10",
  "enable_notifications": "false"
}
```

**Response:** 200 OK

## Error Responses

All endpoints may return the following error responses:

### 400 Bad Request
```json
{
  "error": "Invalid request data"
}
```

### 404 Not Found
```json
{
  "error": "Resource not found"
}
```

### 500 Internal Server Error
```json
{
  "error": "Internal server error"
}
```

## Rate Limiting

Currently no rate limiting is implemented. Future versions may include rate limiting for API protection.

## SDKs and Libraries

Official SDKs are planned for:
- Python
- JavaScript/Node.js
- Java
- C#

## Changelog

### v1.0.0
- Initial API release
- Basic CRUD operations for downloads and categories
- History and statistics endpoints
- Settings management

## Support

For API support and questions:
- Documentation: https://github.com/aerab243/ldm/wiki
- GitHub Issues: https://github.com/aerab243/ldm/issues
- Email: aerabenandrasana@gmail.com

---

*API Version: 1.0.0 | Last Updated: 2025-01-15*