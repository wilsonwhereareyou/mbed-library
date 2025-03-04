const addon = require('./build/Release/newsupport')

function SupportPackage() {

    this.CreateNumpadCommand = function (updateId, outbuffer) {
        return _addonInstance.CreateNumpadCommand(updateId, outbuffer)
    }

    var _addonInstance = new addon.NewSupport()
    // var _addonInstance = new addon.SupportPackage()
}

module.exports = SupportPackage