//shinytrem additions

textures/atcs/eq2_floor_05
{
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/atcs/eq2_floor_05.jpg
		blendFunc filter
	}
	{
		map $whiteimage
		blendFunc GL_DST_COLOR GL_SRC_ALPHA
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
