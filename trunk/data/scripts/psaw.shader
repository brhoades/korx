models/weapons/psaw/psaw
{
	cull disable
	{
		map models/weapons/psaw/psaw.jpg
		rgbGen lightingDiffuse
	}
	{
		map models/weapons/psaw/psaw.jpg
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
		map models/weapons/psaw/psaw_glow.tga
		blendfunc add
		rgbGen wave noise 0.2 0.5 0 3.17 
	}
	{
		map models/weapons/psaw/psaw_glow.tga
		blendfunc add
		rgbGen wave sawtooth 0 0.15 0 3.17 
	}
	{
		map models/weapons/psaw/psaw_glow.tga
		blendfunc add
		rgbGen wave sin 0.25 0.25 0 0.17 
	}
}

models/weapons/psaw/chain
{
	sort additive
	cull disable
	{
		map models/weapons/psaw/chain.jpg
		blendfunc GL_ONE GL_ONE
		tcMod scroll 1.0 -4.0
	}
}

models/weapons/psaw/battery
{
	sort additive
	cull disable
	{
		map models/weapons/psaw/chain.jpg
		blendfunc GL_ONE GL_ONE
		tcMod scroll 0.04 -0.02
	}
}

models/weapons/psaw/glow
{
	cull disable
	{
		map models/weapons/psaw/glow.jpg
		blendfunc GL_ONE GL_ONE
		tcMod scroll -9.0 9.0
	}
}

