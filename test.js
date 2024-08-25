//Test file
const jpakeAddon = require('bindings')('jpakeaddon')
let values = ['0', '1', '2', '3', '4', '5', '6']
let buf = new ArrayBuffer(7)
let view = new Uint8Array(buf)
for (let i = 0; i < 7; i++) {
    view[i] = values[i]
}

jpakeAddon.Setup(view.buffer)
//console.log()

let buf2 = new ArrayBuffer(330)
console.log(buf2)
jpakeAddon.BuildRoundMessage(0, 0, buf2)

console.log(buf2)
