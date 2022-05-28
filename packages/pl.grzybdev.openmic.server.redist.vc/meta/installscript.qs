function Component()
{

}

Component.prototype.createOperations = function()
{
    component.createOperations();
    component.addElevatedOperation("Execute", "{0,3010,1638,5100}", "@TargetDir@\\VC_redist.x64.exe", "/install", "/quiet", "/passive", "/norestart")
}
