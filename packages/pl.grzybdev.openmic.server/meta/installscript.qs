function Component()
{

}

Component.prototype.createOperations = function()
{
    component.createOperations();

    component.addOperation("CreateShortcut",
                           "@TargetDir@/OpenMicServer.exe",
                           "@StartMenuDir@/OpenMic.lnk",
                           "workingDirectory=@TargetDir@",
                           "iconPath=@TargetDir@/OpenMicServer.exe",
                           "iconId=0",
                           "description=OpenMic Server");
}
