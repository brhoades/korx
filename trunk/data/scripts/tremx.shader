gfx/invis/invis
{
  deformvertexes bulge 7 7 8
  {
    map gfx/invis/light.jpg
    tcMod rotate 15
    tcmod stretch sin 0.5 0.05 0 0.05
    rgbGen wave sin 0.1 0.1 0 0.1
    blendFunc add
  }
  {
    map gfx/invis/dark.jpg
    blendFunc gl_zero gl_one_minus_src_color
    tcMod rotate -10
    tcmod stretch sin 0.5 0.05 0 0.05
    rgbGen wave sin 1 1 0.5 0.1
  }
}
gfx/invis/fade
{
  deformvertexes bulge 3 3 8
  {
    map models/players/level1/level1upg.jpg
    blendFunc GL_ONE GL_ONE
    rgbgen wave sin 0.0 1.0 0.25 0.5
  }
  {
    map gfx/invis/light.jpg
    tcMod rotate 15
    tcmod stretch sin 0.5 0.05 0 0.05
    rgbGen wave sin 0.1 0.1 0 0.1
    blendFunc add
  }
  {
    map gfx/invis/dark.jpg
    blendFunc gl_zero gl_one_minus_src_color
    tcMod rotate -10
    tcmod stretch sin 0.5 0.05 0 0.05
    rgbGen wave sin 1 1 0.5 0.1
  }
}

gfx/invis/ateam
{
  deformvertexes bulge 7 7 8
  {
    map gfx/invis/light.jpg
    tcMod rotate 15
    tcmod stretch sin 0.5 0.05 0 0.05
    rgbGen wave sin 0.1 0.1 0 0.1
    blendFunc add
  }
  {
    map gfx/invis/dark.jpg
    blendFunc gl_zero gl_one_minus_src_color
    tcMod rotate -10
    tcmod stretch sin 0.5 0.05 0 0.05
    rgbGen wave sin 1 1 0.5 0.1
  }
  {
    map gfx/invis/ateam.tga
    tcMod rotate 90
    tcmod stretch sin 0.5 0.05 0 0.05
    blendFunc add
  }
  {
    map gfx/invis/ateam.tga
    tcMod rotate -90
    tcmod stretch sin 0.5 0.05 0 0.05
    blendFunc add
  }
}

gfx/invis/hteam
{
  deformvertexes bulge 7 7 8
  {
    map gfx/invis/light.jpg
    tcMod rotate 15
    tcmod stretch sin 0.5 0.05 0 0.05
    rgbGen wave sin 0.1 0.1 0 0.1
    blendFunc add
  }
  {
    map gfx/invis/dark.jpg
    blendFunc gl_zero gl_one_minus_src_color
    tcMod rotate -10
    tcmod stretch sin 0.5 0.05 0 0.05
    rgbGen wave sin 1 1 0.5 0.1
  }
  {
    map gfx/invis/hteam.tga
    tcMod rotate 90
    tcmod stretch sin 0.5 0.05 0 0.05
    blendFunc add
  }
  {
    map gfx/invis/hteam.tga
    tcMod rotate -90
    tcmod stretch sin 0.5 0.05 0 0.05
    blendFunc add
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
  deformVertexes wave 6 sin 0 2 0 8
  {
    map models/buildables/forcefield/forcefield_energy_1.tga
    tcMod Scale 1 2
    tcMod Scroll 0 7
    blendFunc add
  }
  {
    map models/buildables/forcefield/forcefield_energy_1.tga
    tcMod Scale 1 2
    tcMod Scroll 0 8
    blendFunc add
  }
  {
    map models/buildables/forcefield/forcefield_energy_2.tga
    tcMod Scale 1 2
    tcMod Scroll 0 9
    blendFunc add
  }
  {
    map models/buildables/forcefield/forcefield_energy_2.tga
    tcMod Scale 1 2
    tcMod Scroll 0 10
    blendFunc add
  }
  {
    map models/buildables/forcefield/forcefield_energy_3.tga
    tcMod Scale 1 3
    tcMod Scroll 0 4
    blendFunc add
  }
  {
    map models/buildables/forcefield/forcefield_energy_3.tga
    tcmod Scale 1 3
    tcMod Scroll 0 -4
    blendFunc add
  }
}


