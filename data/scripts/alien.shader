models/players/builder/builder
{
	{
		map models/players/builder/builder.jpg
		rgbGen lightingDiffuse
	}
	{
		map models/players/builder/builder.jpg
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

//Not sure if this is used!
models/players/bgranger/bgranger
{
	{
		map models/players/bgranger/bgranger.jpg
		rgbGen lightingDiffuse
	}
	{
		map models/players/bgranger/bgranger.jpg
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

models/players/level0/level0
{
	{
		map models/players/level0/level0.jpg
		rgbGen lightingDiffuse
	}
	{
		map models/players/level0/level0.jpg
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
models/players/level0/level0upg
{
  {
    map models/players/level0/level0upg.jpg
    blendFunc add
    alphaGen  identity
    rgbgen identity
  }
}
models/players/level1/level1
{
	{
		map models/players/level1/level1.jpg
		rgbGen lightingDiffuse
	}
	{
		map models/players/level1/level1.jpg
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

models/players/level1/level1upg
{
  {
    map models/players/level1/level1upg.jpg
    blendFunc GL_ONE GL_ONE
    //alphaGen  identity
    //rgbgen wave sin 0.0 1.0 0.0 1.0
  }
  {
    map gfx/invis/light.jpg
    blendFunc GL_ONE GL_ONE
    tcmod scale 2 2
    tcMod scroll 0.2 -0.2
  }
}

models/players/level2/default
{
	{
		map models/players/level2/default.tga
		rgbGen lightingDiffuse
	}
	{
		map models/players/level2/default.tga
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

models/players/level3/level3
{
	{
		map models/players/level3/level3.jpg
		rgbGen lightingDiffuse
	}
	{
		map models/players/level3/level3.jpg
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

models/players/level3/level3adv
{
	{
		map models/players/level3/level3adv.tga
		rgbGen lightingDiffuse
	}
	{
		map models/players/level3/level3adv.tga
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

models/players/level4/level4
{
	{
		map models/players/level4/level4.jpg
		rgbGen lightingDiffuse
	}
	{
		map models/players/level4/level4.jpg
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
