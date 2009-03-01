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
    map models/players/level1/level1upg.jpg
    blendFunc GL_ONE GL_ONE
    rgbgen wave sin 0.0 1.0 0.25 0.5
  }
        {
                map gfx/invis.jpg
                tcMod rotate 15
                tcmod stretch sin 0.5 0.05 0 0.05
                rgbGen wave sin 0.1 0.1 0 0.1
                blendFunc add
        }
        {
                map gfx/invis_b.jpg
                blendFunc gl_zero gl_one_minus_src_color
                tcMod rotate -10
                tcmod stretch sin 0.5 0.05 0 0.05
                rgbGen wave sin 1 1 0.5 0.1
        }
}
gfx/invis
{
        {
                map gfx/invis.jpg
                tcMod rotate 15
                tcmod stretch sin 0.5 0.05 0 0.05
                rgbGen wave sin 0.1 0.1 0 0.1
                blendFunc add
        }
        {
                map gfx/invis_b.jpg
                blendFunc gl_zero gl_one_minus_src_color
                tcMod rotate -10
                tcmod stretch sin 0.5 0.05 0 0.05
                rgbGen wave sin 1 1 0.5 0.1
        }
}

models/buildables/forcefield/energy
{
  cull disable
	{
		map models/buildables/repeater/energy.tga
		rgbGen wave sawtooth 0.1 0.1 0 0.5 
    blendFunc GL_ONE GL_ONE
		tcMod scale 1 1
		tcMod scroll 0 1
	}
	{
		map models/buildables/forcefield/forcefieldenergy.jpg
		tcMod Scroll .1 0
		blendFunc add
	}
	{
		map models/buildables/forcefield/forcefieldenergy_grid.jpg
		tcMod Scroll -.01 0
		blendFunc add
		rgbgen wave sin .2 .2 0 .4
	}
}


