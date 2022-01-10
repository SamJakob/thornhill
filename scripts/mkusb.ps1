param(
    [string]$image = "./out/thornhill.img"
);

# Print header.
Write-Host "";
Write-Host "Thornhill | USB Creator for Microsoft Windows Development Machines";
Write-Host "Developed by NBTX";
Write-Host "";

# Imports
Add-Type -AssemblyName System.Windows.Forms

# Ensure that dd is present on the system.
$dd = "C:\Program Files\Git\usr\bin\dd.exe";
$ddExists = [System.IO.File]::Exists($dd);
if (!$ddExists) {
    Write-Host "Unable to locate dd.exe. It is commonly installed by Git for Windows." -ForegroundColor Red;
    Write-Host "";
    Write-Host "If you have it installed, you can select its location (though you may want to update the path in the script.)" -ForegroundColor Red;
    Write-Host "Otherwise, you will need to install it." -ForegroundColor Red;
    Write-Host "";
    
    $locateOption = New-Object System.Management.Automation.Host.ChoiceDescription '&Locate', 'Locate';
    $exitOption = New-Object System.Management.Automation.Host.ChoiceDescription '&Exit', 'Exit';

    $options = [System.Management.Automation.Host.ChoiceDescription[]]($locateOption, $exitOption);
    $title = "Unable to locate dd.exe";
    $message = "Please select one of the following resolutions.";
    $result = $host.ui.PromptForChoice($title, $message, $options, 0);

    $FileBrowser = New-Object System.Windows.Forms.OpenFileDialog -Property @{
        InitialDirectory = [Environment]::GetFolderPath('Desktop')
        Filter = 'Executable Programs (*.exe)|*.exe'
    }

    switch ($result) {
        # Locate
        0 {
            $FileBrowser.ShowDialog();
            $dd = $FileBrowser.FileName;
        }

        # Exit
        1 { Exit }
    }
}

<#
# Get a list of all the drives on the system.
Write-Host "Enumerating drives...";
$drives = Get-WmiObject win32_diskdrive -Filter "InterfaceType = 'USB'" | Select Caption,DeviceID,BytesPerSector,InterfaceType,BitmapSize;
$drive;

if (-not $drives) {
    
    Write-Host "No viable target devices were found." -ForegroundColor Red;
    Exit;

}

if ($drives -is [array]) {

    # Display a window for the user to select the intended drive.    
    $drive = $drives | Out-GridView -OutputMode Single -Title "Please select the intended target drive...";

} else {

    # If there is only one valid drive, it is automatically selected instead of an array.
    $drive = $drives;

    Write-Host "Only one viable target device was found, so it was automatically selected.";

}

# Confirm that the user wants to perform the actions.
Write-Host "Don't worry, no data will be written until the operation is confirmed." -ForegroundColor Green;

Write-Host "";
Write-Host "If you continue, $($image) will be written to the following $($drive.InterfaceType) device: $($drive.Caption) ($($drive.DeviceID)).";
$confirmation = Read-Host "Do you wish to continue? (y/N)"

if (!($confirmation.ToLower() -eq 'y')) {
    Write-Host "Operation cancelled." -ForegroundColor Red;
    Exit;
}

#>

Write-Host "If you continue, $($image) will be written to /dev/sdd1.";
Write-Host "You may want to cat /proc/partitions first.";
Write-Host "";
$confirmation = Read-Host "Do you wish to continue? (y/N)"

if (!($confirmation.ToLower() -eq 'y')) {
    Write-Host "Operation cancelled." -ForegroundColor Red;
    Exit;
}

# Perform the actions.
$ddParameters = "if=$($image)", "of=/dev/sdd1", "bs=102400", "status=progress"
& $dd $ddParameters
