models/weapons/xael/xael
{
	cull disable
	{
		map models/weapons/xael/xael.jpg
		rgbGen lightingDiffuse
	}
	{
		map models/weapons/xael/xael.jpg
		blendFunc GL_SRC_ALPHA GL_ONE
		detail
		alphaGen lightingSpecular
	}
	{
		map models/buildables/mgturret/ref_map.jpg
		blendFunc GL_DST_COLOR GL_ONE
		detail
		tcGen environment
	}
	{
		map models/weapons/xael/xael_glow.tga
		blendfunc add
		rgbGen wave noise 0.2 0.5 0 3.17 
	}
	{
		map models/weapons/xael/xael_glow.tga
		blendfunc add
		rgbGen wave sawtooth 0 0.15 0 3.17 
	}
	{
		map models/weapons/xael/xael_glow.tga
		blendfunc add
		rgbGen wave sin 0.25 0.25 0 0.17 
	}
}

models/weapons/xael/glow
{

	cull disable
	{
		map models/weapons/xael/glow.jpg
		blendfunc GL_ONE GL_ONE
		tcMod scroll -9.0 9.0
	}
}

models/weapons/xael/flash
{
  sort additive
  cull disable
  {
    map	models/weapons/xael/flash.jpg
    blendfunc GL_ONE GL_ONE
  }
}

gfx/xael/primary
{  
  cull disable
  {
    animmap 24 gfx/xael/primary_1.jpg gfx/xael/primary_2.jpg gfx/xael/primary_3.jpg gfx/xael/primary_4.jpg
    blendFunc GL_ONE GL_ONE
  }
}

models/weapons/xael/trail_s
{
	cull disable
	{
		map models/weapons/xael/bolt.tga
		blendfunc add
		rgbGen vertex
		tcMod scroll 0.2 0
	}
	{
		map models/weapons/xael/bolt.tga
		blendfunc add
		rgbGen wave sin 0 1 0 5 
		tcMod scroll 0.5 0
		tcMod scale -1 1
	}
}
