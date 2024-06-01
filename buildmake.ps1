param (
    [Parameter(Mandatory=$true, Position=0)]
    [string]$command
)
switch ($command) {
    "release" {
        make config=release all;
        & ".\target\dis.randword\x86_64\Release\randword.exe";
    }
    "build_run" {
        make config=debug all;
        & ".\target\dis.randword\x86_64\Debug\randword.exe";
    }
    "build" {
        make config=debug all;
    }
    default {
        Write-Host "Invalid command. Please use 'release', 'build_run', or 'build'."
    }
}
