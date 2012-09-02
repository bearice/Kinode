fs = require 'fs'

class Input
    constructor: (@path,@name)->
        fs.open @path,'r', 0o644,(err,@fd)=>
            if err
                console.info err
            else
                @init()

    init: ->
        fs.read @fd,@buf,0,16,null,(err,length,data)=>
            console.info err if err
            @parse data
            @init()

    parse: (data)->
        evt =
            tv_sec  : data.readUInt32LE(0)
            tv_usec : data.readUInt32LE(4)
            type    : data.readUInt16LE(8)
            code    : data.readUInt16LE(10)
            value   : data.readUInt32LE(12)

        console.info evt

module.exports=Input

#i1 = new Input('/dev/input/event0','mxckpd')

