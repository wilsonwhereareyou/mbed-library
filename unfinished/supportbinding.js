const addon = require('./build/Release/newsupport')

function SupportPackage() {
    // this.SetupEncryption = function (appKey) {
    //     return _addonInstance.SetupEncryption(appKey)
    // }
    // this.SetFileData = function (fileBuffer, fileLength, dataType) {
    //     return _addonInstance.SetFileData(fileBuffer, fileLength, dataType)
    // }
    // this.GetFileData = function (outputBuffer) {
    //     return _addonInstance.GetFileData(outputBuffer)
    // }
    // this.ResetFileData = function () {
    //     return _addonInstance.ResetFileData()
    // }
    this.CreateNumpadCommand = function (updateId, outbuffer) {
        return _addonInstance.CreateNumpadCommand(updateId, outbuffer)
    }

    var _addonInstance = new addon.NewSupport()
    // var _addonInstance = new addon.SupportPackage()
}

module.exports = SupportPackage