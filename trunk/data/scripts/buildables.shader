models/buildables/building
{
  //temp shader
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
