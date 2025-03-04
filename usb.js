const { SerialPort } = require('serialport')
const jpakeAddon = require('bindings')('jpakeaddon')
// const SupportPackage = require("./supportbinding.js")
const fs = require('node:fs')
const { off } = require('node:process')

const TANDEM_VID = "0483"
const TANDEM_PID = "5740"
const USB_PACKET_DELIMITER = 0x55
const SHOW_NUMPAD_CMD = 220
const JPAKE_ROUND_REQUEST = 216
const JPAKE_ROUND_RESPONSE = 217
const KEY_CONFIRM_REQUEST = 218
const KEY_CONFIRM_RESPONSE = 219
const KEY_CONFIRM_REQUEST_SIZE = 18
const KEY_CONFIRM_RESPONSE_SIZE = 17
const STK_ENTRY_ET = 236
const USB_MIN_MSG_LENGTH = 9
const JPAKE_ROUND1 = 0
const JPAKE_ROUND2 = 1
const JPAKE_PHASE0 = 0
const JPAKE_PHASE1 = 1
const JPAKE_PHASE2 = 2
const JPAKE_REQUEST_DATA = 0
const JPAKE_SEND_DATA = 1
const JPAKE_REQUEST_HEADER_SIZE = 3
const JPAKE_PHASE_SIZE = 165
const LEN_EC_CURVE = 3
const JPAKE_RESPONSE_HEADER_SIZE = 4  //(phase, step,round, error)
const APPKEY_SIZE = 32
const KEY_CONFIRM_LENGTH = 8
const KEY_CONFIRM_HOST = 0
const KEY_CONFIRM_PUMP = 1
const MAX_FILE_DATA = 1000
const MSG_CARGO_MAX_BYTES = 246

class Message {
    constructor(type, messageId, payload, checksum) {
        this.type = type //Rx or Tx
        this.messageId = messageId
        this.payload = payload
        this.checksum = checksum
    }
}

let jpakeInfo = {
    step: JPAKE_REQUEST_DATA,
    round: JPAKE_ROUND1,
    phase: JPAKE_PHASE0,
    appKey: new Uint8Array(APPKEY_SIZE)
}

//let fileNameOne = ''
let tandemPort;
let txBuff = new Uint8Array(255)
let rxBuff = new Uint8Array(330)
let JPakeRxBuffer = new Uint8Array(330)
let JPakeTxBuffer = new Uint8Array(330)
let messagesLog = []
let decodedChallenge;
// let updateID = [52, 51, 51, 3]
// let keyConfirmed = false
// let supportPackage = new SupportPackage()

const listPorts = () => {
    SerialPort.list().then((ports) => {
        ports.forEach((port) => {
            console.log(port.path)
        })
    })
}

const getTandemDevice = async (cb) => {
    await SerialPort.list().then((ports) => {
        ///console.log(ports)
        ports.forEach((port) => {
            if (port.vendorId === TANDEM_VID && port.productId === TANDEM_PID) {
                console.log(port)
                tandemPort = new SerialPort({
                    path: port.path,
                    baudRate: 9600
                })
                
                return tandemPort
            }
        })
        cb()
    })
}

const sendNumpadCmd = async () => {
    txBuff = []
    let offset = 0
    let checksum = 0
    let cargo = [SHOW_NUMPAD_CMD, 0x02]

    txBuff[offset++] = USB_PACKET_DELIMITER
    txBuff[offset++] = SHOW_NUMPAD_CMD //type
    txBuff[offset++] = 0x01 //length of cargo
    txBuff[offset++] = 0x02 //data G7 numpad

    // add zero timestamp
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;

    // add checksum
    checksum = calcChecksum(txBuff, 0, offset)
    txBuff[offset++] = (checksum >> 8)
    txBuff[offset++] = (checksum & 0xFF)

    let status = jpakeAddon.Setup()
    console.log("Setup: ", status)


    // Here is where we set what we want to happen when data is received back
    tandemPort.on('data', (data) => {
        console.log('got data')
        let message = parseData(data)
        console.log(message)

        if (message !== undefined) {
            switch (data[1]) {
                case STK_ENTRY_ET:
                    sendJpakeRound(JPAKE_REQUEST_HEADER_SIZE)
                    break;
                case JPAKE_ROUND_RESPONSE:
                    //let jpakeError = message.payload[3]
                    handleJpakeResponse(message.payload)
                    break
                case KEY_CONFIRM_RESPONSE:
                    handleKeyConfirmResponse(message.payload)
                    break
                // case CGM_SOFTWARE_UPDATE_FILE_STATUS:
                //     if (message.payload[0] != 0) {
                //         console.log("error: " + message.payload[0])
                //     }
                //     else {
                //         if (bytesSent <= encryptedFileSize) {
                //             sendFile()
                //             //Gonna send another chunk
                //         }
                //         else {
                //             console.log("done sending file")
                //         }
                //     }
                //     break;
            }
        }



    })

    await tandemPort.write(txBuff)

    let newMessage = new Message('Tx', SHOW_NUMPAD_CMD, cargo, checksum)
    messagesLog.push(newMessage)
}

const sendJpakeRound = (payloadSize) => {
    let offset = 0
    let checksum = 0
    let cargo = new Array(165).fill(0)
    let start = jpakeInfo.phase == JPAKE_PHASE1 ? 165 : 0
    let end = jpakeInfo.phase == JPAKE_PHASE2 ? 168 : 165

    //Reset buffer
    txBuff.fill(0)

    txBuff[offset++] = USB_PACKET_DELIMITER
    txBuff[offset++] = JPAKE_ROUND_REQUEST //type



    if (jpakeInfo.step == JPAKE_REQUEST_DATA) {


        txBuff[offset++] = payloadSize //length of cargo
        txBuff[offset++] = jpakeInfo.phase //Phase
        txBuff[offset++] = JPAKE_REQUEST_DATA //Send or request data
        txBuff[offset++] = jpakeInfo.round //Round
    }
    else {
        txBuff[offset++] = payloadSize
        txBuff[offset++] = jpakeInfo.phase
        txBuff[offset++] = JPAKE_SEND_DATA
        txBuff[offset++] = jpakeInfo.round


        // Copy values to send to pump
        JPakeTxBuffer.slice(start, start + end).forEach((value) => {
            txBuff[offset++] = value
        })
        cargo = txBuff.slice(2, offset - 1)
    }

    // add zero timestamp
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;

    // add checksum
    checksum = calcChecksum(txBuff, 0, offset)
    txBuff[offset++] = (checksum >> 8)
    txBuff[offset++] = (checksum & 0xFF)

    tandemPort.write(txBuff)
    let newMessage = new Message('Tx', JPAKE_ROUND_REQUEST, cargo, checksum)
    console.log(newMessage)
    messagesLog.push(newMessage)
}


// Take in data from Serialport and parse into Jpake Header and payload
const parseData = (data) => {
    let offset = 0
    let firstOffset = 0
    let message = undefined
    rxBuff = data

    // Don't do anything if buffer is less than complete message
    while ((rxBuff.length - offset) >= USB_MIN_MSG_LENGTH) {
        message = {}
        let calculatedChecksum

        //console.log('Parsing Incoming USB message')
        firstOffset = offset

        //Make sure first byte is delimeter, 
        if (USB_PACKET_DELIMITER === rxBuff[offset++]) {
            message.type = rxBuff[offset++]
            message.payloadSize = rxBuff[offset++]

            if (message.payloadSize !== 0) {
                //This should be 165 bytes for Jpake messages
                message.payload = rxBuff.slice(offset, message.payloadSize + offset)
            }
            offset += message.payloadSize

            if (rxBuff.length >= (message.payloadSize + USB_MIN_MSG_LENGTH)) {
                // Who cares about timestamp?
                offset += 4

                // The last two bytes are the CRC in little endian
                message.checksum = (rxBuff[offset++] << 8)
                message.checksum += rxBuff[offset++]

                calculatedChecksum = calcChecksum(rxBuff, firstOffset, offset - 2)

                if (message.checksum == calculatedChecksum) {
                    console.log('checksums match')
                }
                else {
                    console.log('checksums do not match')
                }
                rxBuff = rxBuff.slice(offset, rxBuff.length)
                offset = 0
            }

        }



    }

    if (message !== undefined) {
        messagesLog.push(message)
    }

    return message
}

const calcChecksum = (buff, start, end) => {
    let checksum = 0
    buff = buff.slice(start, end)

    for (let i = 1; i < buff.length; i++) {
        checksum += buff[i]
    }
    checksum = checksum & 0xFFFF
    return checksum
}

const handleJpakeResponse = (payload) => {
    let status = 0

    if (jpakeInfo.phase == JPAKE_PHASE0) {
        if (jpakeInfo.step == JPAKE_REQUEST_DATA) {
            // if (jpakeHeader.error != 0) {
            //     console.log(jpakeHeader.error)
            //     return
            // }
            JPakeRxBuffer.set(payload.slice(4))

            status = jpakeAddon.ValidateRoundMessage(jpakeInfo.phase, JPakeRxBuffer.buffer)
            console.log('Validate Phase 0, Status: ' + status)

            if (status != 0) {
                return
            }

            rxBuff = rxBuff.fill(0)
            jpakeInfo.step = JPAKE_SEND_DATA

            // status = jpakeAddon.ValidateRoundMessage(jpakeInfo.phase, JPakeRxBuffer.buffer)
            // console.log('Build Phase 0 message, Status: ' + status)

            if (status != 0) {
                return
            }

            jpakeAddon.BuildRoundMessage(jpakeInfo.round, jpakeInfo.phase, JPakeTxBuffer.buffer)
            //console.dir(JPakeTxBuffer, { 'maxArrayLength': null })
            txBuff.fill(0)
            sendJpakeRound(JPAKE_REQUEST_HEADER_SIZE + JPAKE_PHASE_SIZE)
        }
        else {
            // if (jpakeHeader.error != 0) {
            //     console.log('Phase 0 last recieve failed')
            //     return
            // }
            if (payload[3] != 0) {
                console.log('error: ' + payload[3])
                return
            }

            jpakeInfo.phase = JPAKE_PHASE1
            jpakeInfo.step = JPAKE_REQUEST_DATA
            sendJpakeRound(JPAKE_REQUEST_HEADER_SIZE)
        }
    }
    else if (JPAKE_PHASE1 == jpakeInfo.phase) {
        // We sent a request for data after phase 0, here it is
        if (jpakeInfo.step == JPAKE_REQUEST_DATA) {
            JPakeRxBuffer.set(payload.slice(4), 165)

            //console.dir(JPakeRxBuffer.buffer, { 'maxArrayLength': null })

            status = jpakeAddon.ValidateRoundMessage(jpakeInfo.phase, JPakeRxBuffer.buffer)
            console.log('Validate Phase 1, Status: ' + status)

            jpakeInfo.step = JPAKE_SEND_DATA
            status = jpakeAddon.BuildRoundMessage(jpakeInfo.round, jpakeInfo.phase, JPakeTxBuffer.slice(165).buffer)
            console.log('Build Phase 1 message, Status: ' + status)

            sendJpakeRound(JPAKE_REQUEST_HEADER_SIZE + JPAKE_PHASE_SIZE)
        }
        else {
            if (payload[3] != 0) {
                console.log('error round 1 phase 1: ' + payload[3])
                return
            }

            jpakeInfo.phase = JPAKE_PHASE2
            jpakeInfo.step = JPAKE_REQUEST_DATA
            jpakeInfo.round = JPAKE_ROUND2

            JPakeRxBuffer.fill(0)
            JPakeTxBuffer.fill[0]

            sendJpakeRound(JPAKE_REQUEST_HEADER_SIZE)

            //Tx and rx data is going to be 165 + len_curve_info (3) size
        }
    }
    else if (jpakeInfo.phase == JPAKE_PHASE2) {
        if (jpakeInfo.step == JPAKE_REQUEST_DATA) {
            JPakeRxBuffer.set(payload.slice(JPAKE_RESPONSE_HEADER_SIZE))
            console.log('Got info from pump in phase 2')


            status = jpakeAddon.ValidateRoundMessage(jpakeInfo.phase, JPakeRxBuffer.buffer)
            console.log('Validate Phase 2, Status: ' + status)

            status = jpakeAddon.BuildRoundMessage(jpakeInfo.round, jpakeInfo.phase, JPakeTxBuffer.buffer)
            console.log('Build Phase 2 message, Status: ' + status)

            jpakeInfo.step = JPAKE_SEND_DATA

            sendJpakeRound(JPAKE_REQUEST_HEADER_SIZE + JPAKE_PHASE_SIZE + LEN_EC_CURVE) //171

        }
        else {
            let keylen = 0
            status = jpakeAddon.DeriveKeyMaterial(JPakeTxBuffer.buffer, LEN_EC_CURVE + JPAKE_PHASE_SIZE, keylen)
            console.log('Derive Key Status: ' + status)
            console.log(JPakeTxBuffer)

            jpakeInfo.appKey = JPakeTxBuffer.slice(0, 32)

            sendKeyChallenge()

        }
    }
}

//KeyConfirmRequestMessage = keyRound(1), hostRandomDataEncrypted(8), pumpRandomEncr(8),
// errorcode(1) total 18 bytes
const sendKeyChallenge = () => {
    let status = 0
    let keyRound = KEY_CONFIRM_HOST
    let hostRandomDataEncrypted = new Uint8Array(KEY_CONFIRM_LENGTH)
    let challengeData = new Uint8Array(2 * KEY_CONFIRM_LENGTH)
    decodedChallenge = new Uint8Array(KEY_CONFIRM_LENGTH)

    // Fill the keyChallenge with random Data
    if (0 != jpakeAddon.GetRandomBytes(hostRandomDataEncrypted.buffer, KEY_CONFIRM_LENGTH)) {
        console.log('failed to get random bytes')
    }

    hostRandomDataEncrypted.forEach((value, index) => {
        decodedChallenge[index] = value
        challengeData[index] = value
    })

    //Encrypt the payload
    status = jpakeAddon.GenerateEncryptedChallenge(jpakeInfo.appKey.buffer,
        challengeData.buffer, 16, challengeData.buffer)
    console.log('Generate Encrypted Challenge: ' + status)

    txBuff.fill(0)


    //REFACTOR THIS TO HAVE ONE SEND MESSAGE THAT BUILDS THE MESSAGE
    let offset = 0
    let checksum = 0
    let cargo = new Array(KEY_CONFIRM_REQUEST_SIZE)
    cargo.push(keyRound)

    txBuff[offset++] = USB_PACKET_DELIMITER
    txBuff[offset++] = KEY_CONFIRM_REQUEST //type
    txBuff[offset++] = KEY_CONFIRM_REQUEST_SIZE
    txBuff[offset++] = keyRound

    challengeData.forEach((value) => {
        txBuff[offset++] = value
        cargo.push(value)
    })

    txBuff[offset++] = status
    cargo.push(status)

    // add zero timestamp
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;

    // add checksum
    checksum = calcChecksum(txBuff, 0, offset)
    txBuff[offset++] = (checksum >> 8)
    txBuff[offset++] = (checksum & 0xFF)

    tandemPort.write(txBuff)
    let newMessage = new Message('Tx', KEY_CONFIRM_REQUEST, cargo, checksum)
    console.log(newMessage)
    messagesLog.push(newMessage)
}
//KeyConfirmResponseMessage = hostRandomDataEncrypted(8), pumpRandomEncr(8), errorcode(1)
const handleKeyConfirmResponse = (payload) => {
    let challengeDataEnc = new Uint8Array(16)
    challengeDataEnc.set(payload.slice(0, 16))
    // ChallengeDataEnc has the encrypted challenge data from the pump
    let errorcode = payload[16]

    // if (keyConfirmed == true) {
    //     console.log('pump sent back final error: ' + errorcode)
    //     // if (errorcode == 0) {
    //     //     supportPackage.SetupEncryption(jpakeInfo.appKey.buffer)

    //     //     sendFile()
    //     // }

    //     return
    // }
    console.log('errorcode from pump: ' + errorcode)
    let status = 0
    let challengeDataDec = new Uint8Array(8)

    //Here we decrypt challengeDataEnc using the appKey we created and store it in
    // chalengeDataDec

    status = jpakeAddon.DecryptChallenge(jpakeInfo.appKey.buffer, challengeDataEnc.buffer, 16, challengeDataDec.buffer)
    console.log('Decrypt status: ' + status)
    let success = true
    console.log(challengeDataDec)

    challengeDataDec.forEach((value, index) => {
        console.log(value + ' = ' + decodedChallenge[index])
        if (value != decodedChallenge[index]) {
            success = false
        }
    })
    console.log("Success: " + success)

    for (let i = 0; i < 8; i++) {
        challengeDataEnc[i] = 0
    }

    console.log(challengeDataEnc)

    let pumpChallenge = new Uint8Array(16)

    status = jpakeAddon.GenerateEncryptedChallenge(jpakeInfo.appKey.buffer,
        challengeDataDec.buffer, 16, pumpChallenge.buffer)
    console.log('Generate Encrypted Challenge: ' + status)

    console.log('After encryption')
    console.log(pumpChallenge)


    let keyRound = KEY_CONFIRM_PUMP
    // let tempChallenge = 
    // status = jpakeAddon.GenerateEncryptedChallenge(jpakeInfo.appKey.buffer,
    //     challengeData.buffer, 16, challengeData.buffer)

    //REFACTOR THIS TO HAVE ONE SEND MESSAGE THAT BUILDS THE MESSAGE
    let offset = 0
    let checksum = 0
    let cargo = new Array(KEY_CONFIRM_REQUEST_SIZE)
    // cargo.push(keyRound)
    txBuff.fill(0)

    txBuff[offset++] = USB_PACKET_DELIMITER
    txBuff[offset++] = KEY_CONFIRM_REQUEST //type
    txBuff[offset++] = KEY_CONFIRM_REQUEST_SIZE
    txBuff[offset++] = keyRound

    pumpChallenge.forEach((value) => {
        txBuff[offset++] = value
        // cargo.push(value)
    })

    txBuff[offset++] = status
    //cargo.push(status)

    // add zero timestamp
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;
    txBuff[offset++] = 0;

    // add checksum
    checksum = calcChecksum(txBuff, 0, offset)
    txBuff[offset++] = (checksum >> 8)
    txBuff[offset++] = (checksum & 0xFF)

    console.log(txBuff)

    tandemPort.write(txBuff)
    // let newMessage = new Message('Tx', KEY_CONFIRM_REQUEST, cargo, checksum)
    // console.log(newMessage)
    // messagesLog.push(newMessage)


    keyConfirmed = true

}

// let fileBuffer
// let fileSize = 0
// let bytesRead = 0
// let totalBytesRead = 0
// let certType = 4
// let bytesSent = 0

// let sendFileMsgBuffer = new Uint8Array(246)
// let encryptedFileBuffer = new Uint8Array(600)
// let encryptedFileSize = 0

// const CGM_SOFTWARE_UPDATE_TRANSFER = 214
const CGM_SOFTWARE_UPDATE_FILE_STATUS = 215
// const sendFile = () => {

//     if (totalBytesRead >= encryptedFileSize) {
//         //console.log("total Bytes read for file: " + totalBytesRead)
//         if (certType === 4) {
//             certType = 0
//         }
//         else {
//             certType++
//         }
//         if (certType === 0) {

//             fileBuffer = Uint8Array.from(fs.readFileSync('./certs/embalmed/device_cert.bin'))
//             console.log("sending device cert")
//         }
//         else if (certType === 1) {

//             fileBuffer = Uint8Array.from(fs.readFileSync('./certs/embalmed/device_private_key.bin'))
//             console.log("sending private key")
//         }
//         else if (certType === 2) {
//             console.log("sending root cert")
//             fileBuffer = Uint8Array.from(fs.readFileSync('./certs/embalmed/device_root_cert.bin'))
//         }
//         else if (certType === 3) {
//             console.log("sending issuer cert")
//             fileBuffer = Uint8Array.from(fs.readFileSync('./certs/embalmed/issuer_cert.bin'))
//         }
//         else {

//             console.log("done sending certs")
//             return
//         }
//         bytesRead = 0
//         totalBytesRead = 0

//         console.log("Data Type: " + certType)
//         console.log("Unencrypted Length: " + fileBuffer.length)
//         let setStatus = supportPackage.SetFileData(fileBuffer.buffer, fileBuffer.byteLength, certType)
//         console.log("Set Status: " + setStatus)
//         //Get all of the encrypted info
//         let getStatus = supportPackage.GetFileData(encryptedFileBuffer.buffer)
//         console.log("Get status: " + getStatus)
//         let tempArr = new Uint16Array(encryptedFileBuffer.buffer)

//         encryptedFileSize = tempArr[0]
//         let crc = tempArr[1]
//         console.log("CRC: " + crc)
//         console.log("Encrypted Length: " + encryptedFileSize)


//     }

//     if (totalBytesRead + 241 >= encryptedFileSize) {
//         bytesRead = encryptedFileSize - totalBytesRead
//         totalBytesRead = encryptedFileSize
//     }
//     else {
//         totalBytesRead += 241
//         bytesRead = 241
//     }

//     console.log("Bytes read this message: " + bytesRead)
//     console.log("total Bytes read for file: " + totalBytesRead)

//     txBuff.fill(0)

//     let offset = 0
//     let checksum = 0
//     txBuff[offset++] = USB_PACKET_DELIMITER
//     txBuff[offset++] = CGM_SOFTWARE_UPDATE_TRANSFER
//     txBuff[offset++] = bytesRead + 5
//     txBuff[offset++] = encryptedFileBuffer[0]
//     txBuff[offset++] = encryptedFileBuffer[1]
//     txBuff[offset++] = encryptedFileBuffer[3]
//     txBuff[offset++] = encryptedFileBuffer[2]
//     txBuff[offset++] = encryptedFileBuffer[4]

//     for (bytesSent; bytesSent < totalBytesRead; bytesSent++) {
//         txBuff[offset++] = encryptedFileBuffer[bytesSent + 5]
//     }

//     console.log("Bytes sent: " + bytesSent)

//     // add zero timestamp
//     txBuff[offset++] = 0;
//     txBuff[offset++] = 0;
//     txBuff[offset++] = 0;
//     txBuff[offset++] = 0;

//     // add checksum
//     checksum = calcChecksum(txBuff, 0, offset)
//     txBuff[offset++] = (checksum >> 8)
//     txBuff[offset++] = (checksum & 0xFF)

//     // 3: Send the file to the pump in the necessary amount of messages

//     console.log(txBuff)

//     tandemPort.write(txBuff)

// }

// const testCRCValues = () => {
//     let testfilebuffer = fs.readFileSync('./certs/embalmed/device_root_cert.bin')
//     console.dir(testfilebuffer, { 'maxArrayLength': null })
//     let status = supportPackage.SetFileData(testfilebuffer.buffer, 416, 2);
//     let appKey = new Uint8Array(APPKEY_SIZE)
//     let finalbuff = new Uint8Array(500)
//     appKey[10] = 100
//     supportPackage.SetupEncryption(appKey.buffer)
//     status = supportPackage.GetFileData(finalbuff.buffer)
//     //console.dir(finalbuff, { 'maxArrayLength': null })
// }


module.exports = { getTandemDevice, listPorts, sendNumpadCmd }