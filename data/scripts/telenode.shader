models/buildables/telenode/telenode_top
{
	{
		map models/buildables/telenode/telenode_top.tga
		rgbGen lightingDiffuse
	}
	{
		map models/buildables/telenode/telenode_top.tga
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
}

models/buildables/telenode/energy
{
	{
		map models/buildables/telenode/energy.tga
		rgbGen wave inversesawtooth 0.2 0.4 0 1 
		tcMod rotate 10
	}
}
models/buildables/telenode/rep_cyl
{
  cull disable
  {
    map models/buildables/telenode/telenode_sine.tga
    tcmod scroll 2 0
    blendfunc add
    rgbgen wave triangle 0 0.25 0 0.25
  }
  {
    map models/buildables/telenode/telenode_sine.tga
    tcmod scroll 1 0
    blendfunc add
    rgbgen wave triangle 0 0.5 0 0.25
  } 
  {
    map models/buildables/telenode/telenode_sine.tga
    tcmod scroll 0.5 0
    blendfunc add
    rgbgen wave triangle 0 1 0 0.25
  } 
  {
    map models/buildables/telenode/telenode_sine.tga
    tcmod scroll -2 0
    blendfunc add
    rgbgen wave triangle 0 0.25 0.5 0.25
  }
  {
    map models/buildables/telenode/telenode_sine.tga
    tcmod scroll -1 0
    blendfunc add
    rgbgen wave triangle 0 0.5 0.5 0.25
  } 
  {
    map models/buildables/telenode/telenode_sine.tga
    tcmod scroll -0.5 0
    blendfunc add
    rgbgen wave triangle 0 1 0.5 0.25
  }
  {
    map models/buildables/telenode/telenode_base_glow.tga
    blendfunc add
    rgbgen wave sin 0.65 0.25 0 0.5
  }
}

models/buildables/telenode/telenode_parts
{
	{
		map models/buildables/telenode/telenode_parts.tga
		rgbGen lightingDiffuse
	}
	{
		map models/buildables/telenode/telenode_parts.tga
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
}

