#!/usr/bin/env node
B = require('./binding');
K=new B.Kanvas(600,800);
K.fillRect();
//K.color=0x1f;
//K.fillRect(0,0,500,500);

F = new B.Font("方正黑体_GBK.TTF");
console.info(F);
F.size = 64;
K.font = F;
K.color = 0x40;
K.fillRect(64,64,192,192);
K.color = 0xff;

s = "The quick brown fox jumps over the lazy dog"

var j = 0;
for(var i=24;j<=800;){
	F.size = i;
	console.info(K.sizeString(s));
	K.drawString(i + " " + s,0,j);
	j+=i;
}
//K.drawString(s,0,128);

Fb = new B.FBDev();
Fb.update(K);

