models/buildables/building
{
  //temp shader
  deformvertexes bulge 1 1 2
  cull disable
  {
    map models/buildables/building/sine.tga
    blendfunc add
    rgbgen wave inversesawtooth 0.3 0.5 0.35 0.25
    tcmod scroll 0.5 0
  }
  {
    map models/buildables/building/sine.tga
    blendfunc add
    rgbgen wave inversesawtooth 0 0.5 0 0.25
    tcmod scroll -0.5 0
  } 
  {
    map models/buildables/building/sine.tga
    blendfunc add
    rgbgen wave noise 0.2 0.5 0 3.17 
    tcmod scroll 1 0
  }
  {
    map models/buildables/building/sine.tga
    blendfunc add
    rgbgen wave inversesawtooth 0.3 0.5 0.5 0.25
    tcmod scroll -1 0
  }
  {
    map models/buildables/building/glow.tga
    blendfunc add
    rgbgen wave noise 0.65 1 0.5 1
  }
  {
    map models/buildables/building/randomness.tga
    rgbgen wave sawtooth 0.3 0.3 0.5 0.1
    blendFunc GL_ONE GL_ONE
  }
  {
    map models/buildables/building/randomness.tga
    rgbgen wave noise 0.1 0.25 0.3 0.5
    blendFunc GL_ONE GL_ONE
  }
}
