function Component()
{

}

Component.prototype.createOperations = function()
{
    component.createOperations();

    component.addOperation("CreateShortcut",
                           "@TargetDir@/OpenMicServer.exe",
                           "@DesktopDir@/OpenMic.lnk",
                           "workingDirectory=@TargetDir@",
                           "iconPath=@TargetDir@/OpenMicServer.exe",
                           "iconId=0",
                           "description=OpenMic Server");
}
