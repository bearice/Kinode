input = require 'input'
binding = require 'binding'

fb = new binding.FBDev()
fb.clear()

screen = new binding.Kanvas(600,800)

font = new binding.Font("../方正黑体_GBK.TTF")
screen.font = font

string = "The quick brown fox jumps over the lazy dog"

redraw = (size,color)->
	console.info size,color
	screen.color = 0
	screen.fillRect()
	screen.color = Math.abs(color)
	st = (new Date()).valueOf()
	j=0
	i=size
	while j<=800
		font.size = i
		screen.drawString "#{i} #{color} #{string}",0,j
		j+=i
	end =  (new Date()).valueOf()
	fb.update screen
	console.info(end-st)

color = 15
size = 24
input.on 'keyDown' ,(evt)->
	switch evt.code
		when input.Keys.KEY_UP
			size += 1
			redraw size,color
		when input.Keys.KEY_DOWN
			size -= 1
			redraw size,color
		when input.Keys.KEY_RIGHT
			color -= 1
			color &= 0x0f
			redraw size,color
		when input.Keys.KEY_LEFT
			color += 1
			color &= 0x0f
			redraw size,color
		when input.Keys.KEY_BACK
		    fb.clear()
		    process.exit 0

redraw size,color
