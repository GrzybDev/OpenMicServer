function Component()
{

}

Component.prototype.createOperations = function()
{
    component.createOperations();
    component.addElevatedOperation("Execute", "@TargetDir@\\VBCABLE_Setup_x64.exe", "-h", "-i", "-H", "-n")
}
