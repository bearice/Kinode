fs = require 'fs'
events = require 'events'

key_codes = require './key_codes'

class InputEvent
    @::Keys = key_codes
    constructor: (props)->
        (@[key] = val if props.hasOwnProperty key) \
            for key,val of props

    getKeyName: ()->
        key_codes.getName @code
        

class Input extends events.EventEmitter
    @::Keys = key_codes
    @::InputEvent = InputEvent
    constructor: (@devices)->
        @bufs = []
        @fds  = []
        @_openDev name,dev \
            for name,dev of @devices
            
    _openDev: (name,dev) ->
        fd = @fds[name] = fs.openSync dev,'r', 0o644
        buf = @bufs[name] = new Buffer 16
        @init name,fd,buf

    init: (name,fd,buf) ->
        fs.read fd,buf,0,16,null,(err,length,data)=>
            console.info err if err
            @parse name,data
            @init name,fd,buf

    parse: (name,data)->
        evt = new InputEvent
            name    : name
            tv_sec  : data.readUInt32LE(0)
            tv_usec : data.readUInt32LE(4)
            type    : data.readUInt16LE(8)
            code    : data.readUInt16LE(10)
            value   : data.readUInt32LE(12)
        @emit 'keyEvent', evt
        @emit (['keyUp','keyDown','keyHold'])[evt.value],evt


KINDLE_INPUT_DEV =
    'keypad' :'/dev/input/event0'
    'fiveway':'/dev/input/event1'
    'volume' :'/dev/input/event2'

i = new Input KINDLE_INPUT_DEV

if require.main is module
    console.info("Waiting for a key..")
    i.on 'keyEvent',(e)->
        console.info e.getKeyName(), ['up','down','hold'][e.value]
else
    module.exports=i
