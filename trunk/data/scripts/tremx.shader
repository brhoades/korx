models/players/level0/level0adv
{
  {
    map models/players/level0/level0upg.jpg
    blendFunc add
    alphaGen  identity
    rgbgen identity
  }
}
models/players/level1/level1adv
{
  {
    map models/players/level1/level1upg.jpg
    blendFunc GL_ONE GL_ONE
    //alphaGen  identity
    //rgbgen wave sin 0.0 1.0 0.0 1.0
  }
  {
    map gfx/invis.jpg
    blendFunc GL_ONE GL_ONE
    tcmod scale 2 2
    tcMod scroll 0.2 -0.2
  }
}

gfx/invisfade
{
  {
    map models/players/level1/level1upg.tga
    blendFunc GL_ONE GL_ONE
    rgbgen wave sin 0.0 1.0 0.25 0.5
  }
  {
                map gfx/invis.jpg
                tcMod rotate 15
                tcmod stretch sin 0.5 0.2 0 0.05
                rgbGen wave sin 0.1 0.1 0 0.2
                blendFunc add
  }
        {
                map gfx/invis.jpg
                blendFunc gl_zero gl_one_minus_src_color
                tcMod rotate -10
                tcmod stretch sin 0.5 0.2 0 0.12
                rgbGen wave sin 1 0.5 0 0.05
        }
}
gfx/invis
{
        {
                map gfx/invis.jpg
                tcMod rotate 15
                tcmod stretch sin 0.5 0.2 0 0.05
                rgbGen wave sin 0.1 0.1 0 0.2
                blendFunc add
        }
        {
                map gfx/invis.jpg
                blendFunc gl_zero gl_one_minus_src_color
                tcMod rotate -10
                tcmod stretch sin 0.5 0.2 0 0.1
                rgbGen wave sin 1 0.5 0 0.05
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

models/weapons/xael/plasma
{
	sort additive
	cull disable
	{
		map models/weapons/xael/plasma.jpg
		blendfunc GL_ONE GL_ONE
		tcMod scroll 0.4 0.2
	}
}

models/weapons/xael/pilot
{
// sort additive
// cull disable
	{
		map models/weapons/flamer/pilot.jpg
		blendfunc GL_ONE GL_ONE
		tcMod scroll 20.0 0
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

/*
models/weapons/xael/xael
{
	sort additive
	cull disable
	surfaceparm trans
	{
		map models/weapons/xael/xael.tga
		depthWrite
		alphaFunc GE128
		rgbGen lightingDiffuse
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
}
*/

models/weapons/xael/droid
{
	{
		map models/weapons/xael/droid.jpg
		rgbGen lightingDiffuse
	}
	{
		map models/buildables/mgturret/ref_map.tga
		blendfunc filter
		rgbGen identity
    tcMod scale .25 .25
		tcGen environment 
	}
}

models/buildables/forcefield/energy
{
  cull disable
	//{
	//	map models/buildables/repeater/energy.tga
		//rgbGen wave sawtooth 0.3 1 0 0.5 
  //  blendFunc GL_ONE GL_ONE
	//	tcMod scale 2 1
	//	tcMod scroll 0 1
	//}
	{
		map textures/atcs/force_field.tga
		tcMod Scroll .1 0
		blendFunc add
	}
	{
		map textures/atcs/force_grid.tga
		tcMod Scroll -.01 0
		blendFunc add
		rgbgen wave sin .2 .2 0 .4
	}
}


