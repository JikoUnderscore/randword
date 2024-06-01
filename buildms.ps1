param (
    [Parameter(Mandatory=$true, Position=0)]
    [string]$command
)
switch ($command) {
    "release" {
        & msbuild randword\\randword.vcxproj /p:configuration=release /p:platform=x64 -v:Minimal -flp:logfile=randword.log;
        & ".\target\dis.randword\x86_64\Release\randword.exe";
    }
    "build_run" {
        & msbuild randword\\randword.vcxproj /p:configuration=debug /p:platform=x64 -v:Minimal -flp:logfile=randword.log; 
        & ".\target\dis.randword\x86_64\Debug\randword.exe";
    }
    "build" {
        & msbuild randword\\randword.vcxproj /p:configuration=debug /p:platform=x64 -v:Minimal -flp:logfile=randword.log;
    }
    default {
        Write-Host "Invalid command. Please use 'release', 'build_run', or 'build'."
    }
}
