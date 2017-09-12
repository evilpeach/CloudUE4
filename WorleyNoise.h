// Texture 1
float texture1_r_perlin_low     =  0.3;
float texture1_r_perlin_high    =  1.4;
float texture1_r_worley_low     = -0.3;
float texture1_r_worley_high    =  1.3;
float texture1_gba_worley_low   = -0.4;
float texture1_gba_worley_high  =  1.0;

float perlin_r = get_perlin_7_octaves(xyz, 4.0);
float worley_r = get_worley_3_octaves(xyz, 6.0);
float worley_g = get_worley_3_octaves(xyz, 6.0);
float worley_b = get_worley_3_octaves(xyz, 12.0);
float worley_a = get_worley_3_octaves(xyz, 24.0);

// Remap the values
perlin_r = set_range(perlin_r, texture1_r_perlin_low, texture1_r_perlin_high);
worley_r = set_range(worley_r, texture1_r_worley_low, texture1_r_worley_high);
worley_g = set_range(worley_g, texture1_gba_worley_low, texture1_gba_worley_high);
worley_b = set_range(worley_b, texture1_gba_worley_low, texture1_gba_worley_high);
worley_a = set_range(worley_a, texture1_gba_worley_low, texture1_gba_worley_high);

// Combinning the two noises (this is what they refer as "dilating" the perlin noise)
float worley_perlin = dilate_perlin_worley(perlin_r, worley_r, perlin_to_worley_ratio);
					
return float4(worley_perlin, 1.0-worley_g, 1.0-worley_b, 1.0-worley_a);