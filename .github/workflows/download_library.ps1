# Create External/Library directory if it doesn't exist
New-Item -Path "External/Library" -ItemType Directory -Force

$LibraryLink = $env:LIB_DIRECTORY

# Download External Library from Google Drive
if ([string]::IsNullOrEmpty($LibraryLink))
{
    Write-Error "LIB_DIRECTORY secret is not set"
    exit 1
}

# Extract folder ID from Google Drive link
if ($LibraryLink -match "/folders/([a-zA-Z0-9_-]+)")
{
    $FolderID = $matches[1]
}
elseif ($LibraryLink -match "id=([a-zA-Z0-9_-]+)")
{
    $FolderID = $matches[1]
}
else
{
    Write-Error "Invalid Google Drive folder link format. Expected format: https://drive.google.com/drive/folders/FOLDER_ID"
    exit 1
}

Write-Output "Downloading External Library Folder..."

# Use gdown to download the entire folder
gdown --folder "https://drive.google.com/drive/folders/$FolderID" --output "External/Library/" --quiet

if ($LASTEXITCODE -ne 0)
{
    Write-Error "Failed To Download Folder Using gdown"
    exit 1
}

Write-Output "External Library Download Completed"

# List downloaded contents for verification
Write-Output "Downloaded Contents:"
Get-ChildItem -Path "External/Library/" -Recurse | Select-Object FullName
