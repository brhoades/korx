models/buildables/telenode/telenode_top
{
	{
		map models/buildables/telenode/telenode_top.jpg
		rgbGen lightingDiffuse
	}
	{
		map models/buildables/telenode/telenode_top.jpg
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
    map models/buildables/telenode/telenode_ring_3.tga
    blendfunc add
    rgbgen wave inversesawtooth 0 0.2 0 0.5
  }
  {
    map models/buildables/telenode/telenode_ring_2.tga
    blendfunc add
    rgbgen wave inversesawtooth 0 0.3 0.1 0.5
  } 
  {
    map models/buildables/telenode/telenode_ring_1.tga
    blendfunc add
    rgbgen wave inversesawtooth 0 0.4 0.2 0.5
  }
  {
    map models/buildables/telenode/telenode_sine.tga
    blendfunc add
    rgbgen wave inversesawtooth 0 0.5 0 0.25
    tcmod scroll 0.5 0
  }
  {
    map models/buildables/telenode/telenode_sine.tga
    blendfunc add
    rgbgen wave inversesawtooth 0 0.5 0 0.25
    tcmod scroll -0.5 0
  } 
  {
    map models/buildables/telenode/telenode_sine.tga
    blendfunc add
    rgbgen wave inversesawtooth 0 0.5 0.5 0.25
    tcmod scroll 1 0
  }
  {
    map models/buildables/telenode/telenode_sine.tga
    blendfunc add
    rgbgen wave inversesawtooth 0 0.5 0.5 0.25
    tcmod scroll -1 0
  }
  {
    map models/buildables/telenode/telenode_base_glow.tga
    blendfunc add
    rgbgen wave sin 0.65 0.25 0 1
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

