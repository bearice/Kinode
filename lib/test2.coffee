input = require './input'
binding = require './binding'

fb = new binding.FBDev()
fb.clear()

screen = new binding.Kanvas(600,800)

font = new binding.Font("../方正黑体_GBK.TTF")
screen.font = font

string = "The quick brown fox jumps over the lazy dog"

redraw = (size)->
	screen.color = 0
	screen.fillRect()
	screen.color = 0xff
	st = (new Date()).valueOf()
	j=0
	i=size
	while j<=800
		font.size = i
		screen.drawString "#{i} #{string}",0,j
		j+=i
	end =  (new Date()).valueOf()
	fb.update screen
	console.info(end-st)


size = 24
input.on 'keyDown' ,(evt)->
	switch evt.code
		when input.Keys.KEY_VOLUMEUP
			size += 2
			redraw size
		when input.Keys.KEY_VOLUMEDOWN
			size -= 2
			redraw size
		when input.Keys.KEY_BACK
		    fb.clear()
		    process.exit 0

redraw size
