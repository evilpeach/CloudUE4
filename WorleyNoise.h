				x *= s;
				x += 0.5;
				float3 p = floor(x);
				float3 f = frac(x);

				float id = 0.0;
				float2 res = float2( 1.0 , 1.0);
				for(int k=-1; k<=1; k++){
					for(int j=-1; j<=1; j++) {
						for(int i=-1; i<=1; i++) {
							float3 b = float3(i, j, k);
							//voronoi_hash
							
							float3 r = float3(b) - f;// + voronoi_hash(p + b, s);
							float d = dot(r, r);

							if(d < res.x) {
								id = dot(p + b, float3(1.0, 57.0, 113.0));
								res = float2(d, res.x);			
							} else if(d < res.y) {
								res.y = d;
							}
						}
					}
				}

				float2 result = res;//sqrt(res);
				id = abs(id);

				if(inverted == 1)
					return float3(1.0 - result, id);
				else
					return float3(result, id);
			}