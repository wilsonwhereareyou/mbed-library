const express = require('express')
const { getTandemDevice, sendNumpadCmd, sendFile } = require('./usb')

let myData = 'hui'

const app = express()

app.get('/', async function (req, res) {
    //await sendNumpadCmd()

    res.send('Hello World: ' + myData)
})
app.listen(3000, async function () {
    //await getTandemDevice()
    sendFile()
    console.log('App listening on port 3000')
})