export module holefinder;

import std;

import c0surface;
import gregorystructs;
import point;
import patch;
import surface;

using namespace Gregory;

auto findNode(const BorderNode& val, const Border& vec)
{
	return std::find_if(vec.begin(), vec.end(), [&](const auto& node)
	{
		return node.p == val.p;
	});
}

int mod(int val, int modVal)
{
	return ((val % modVal) + modVal) % modVal;
}

export namespace math
{
	/// <summary>
	/// Finds the holes in the given surfaces.
	/// </summary>
	/// <param name="surfaces">A vector containing pointers to the surfaces to check for holes.</param>
	/// <returns>
	/// A vector containing the holes found in the surfaces.
	/// </returns>
	std::vector<Hole> findHole(const std::vector<Surface*>& surfaces)
	{
		for (auto&& surface : surfaces)
		{
			if (dynamic_cast<C0Surface*>(surface) == nullptr)
			{
				return {};
			}
		}

		switch (surfaces.size())
		{
		case 1:
		{
			auto surface = dynamic_cast<C0Surface*>(surfaces[0]);
			if (!surface->isCylinder() || surface->getSizeZ() != 3)
				return {};

			Hole hole0, hole1;
			auto&& nodes = surface->getBorder();
			hole0[0] = HoleEdge{
				HoleNode{nodes[0].p, nodes[0].prevBefore},
				HoleNode{nodes[1].p, nodes[1].prevBefore},
				HoleNode{nodes[2].p, nodes[2].prevBefore},
				HoleNode{nodes[3].p, nodes[3].prevBefore}
			};
			hole0[1] = HoleEdge{
				HoleNode{nodes[3].p, nodes[3].prevBefore},
				HoleNode{nodes[4].p, nodes[4].prevBefore},
				HoleNode{nodes[5].p, nodes[5].prevBefore},
				HoleNode{nodes[6].p, nodes[6].prevBefore}
			};
			hole0[2] = HoleEdge{
				HoleNode{nodes[6].p, nodes[6].prevBefore},
				HoleNode{nodes[7].p, nodes[7].prevBefore},
				HoleNode{nodes[8].p, nodes[8].prevBefore},
				HoleNode{nodes[0].p, nodes[0].prevBefore}
			};

			static constexpr auto offset = 9;
			hole1[0] = HoleEdge{
				HoleNode{nodes[offset + 0].p, nodes[offset + 0].prevBefore},
				HoleNode{nodes[offset + 1].p, nodes[offset + 1].prevBefore},
				HoleNode{nodes[offset + 2].p, nodes[offset + 2].prevBefore},
				HoleNode{nodes[offset + 3].p, nodes[offset + 3].prevBefore}
			};
			hole1[1] = HoleEdge{
				HoleNode{nodes[offset + 3].p, nodes[offset + 3].prevBefore},
				HoleNode{nodes[offset + 4].p, nodes[offset + 4].prevBefore},
				HoleNode{nodes[offset + 5].p, nodes[offset + 5].prevBefore},
				HoleNode{nodes[offset + 6].p, nodes[offset + 6].prevBefore}
			};
			hole1[2] = HoleEdge{
				HoleNode{nodes[offset + 6].p, nodes[offset + 6].prevBefore},
				HoleNode{nodes[offset + 7].p, nodes[offset + 7].prevBefore},
				HoleNode{nodes[offset + 8].p, nodes[offset + 8].prevBefore},
				HoleNode{nodes[offset + 0].p, nodes[offset + 0].prevBefore}
			};

			return { hole0, hole1 };
		}
			
		case 2:
		{
			auto surface0 = dynamic_cast<C0Surface*>(surfaces[0]);
			auto surface1 = dynamic_cast<C0Surface*>(surfaces[1]);
			auto&& border0 = surface0->getBorder();
			auto&& border1 = surface1->getBorder();

			for (auto&& point : border0)
			{
				Hole hole;
				auto foundIn1 = findNode(point, border1);
				if (foundIn1 == border1.end())
					continue;
				int indexIn1 = static_cast<int>(std::distance(border1.cbegin(), foundIn1));

				auto foundIn0Plus3 = findNode(border1[mod(indexIn1 + 3, border1.size())], border0);
				auto foundIn0Minus3 = findNode(border1[mod(indexIn1 + 3, border1.size())], border0);
				auto foundIn0Plus6 = findNode(border1[mod(indexIn1 + 6, border1.size())], border0);
				auto foundIn0Minus6 = findNode(border1[mod(indexIn1 - 6, border1.size())], border0);

				// First surface has two patches
				if (foundIn0Plus3 != border0.end())
				{
					hole[0] = HoleEdge{
						HoleNode{border1[indexIn1].p, border1[indexIn1].prevBefore},
						HoleNode{border1[mod(indexIn1 + 1, border1.size())].p, border1[mod(indexIn1 + 1, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 + 2, border1.size())].p, border1[mod(indexIn1 + 2, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 + 3, border1.size())].p, border1[mod(indexIn1 + 3, border1.size())].prevAfter}
					};

					auto indexIn0 = std::distance(border0.cbegin(), foundIn0Plus3);
					if (border0[mod(indexIn0 + 6, border0.size())].p == point.p)
					{
						hole[1] = HoleEdge{
							HoleNode{border0[indexIn0].p, border0[indexIn0].prevBefore},
							HoleNode{border0[mod(indexIn0 + 1, border0.size())].p, border0[mod(indexIn0 + 1, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 2, border0.size())].p, border0[mod(indexIn0 + 2, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 3, border0.size())].p, border0[mod(indexIn0 + 3, border0.size())].prevAfter}
						};
						hole[2] = HoleEdge{
							HoleNode{border0[mod(indexIn0 + 3, border0.size())].p, border0[mod(indexIn0 + 4, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 4, border0.size())].p, border0[mod(indexIn0 + 4, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 5, border0.size())].p, border0[mod(indexIn0 + 5, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 6, border0.size())].p, border0[mod(indexIn0 + 6, border0.size())].prevAfter}
						};
						return { hole };
					}
					else if (border0[mod(indexIn0 - 6, border0.size())].p == point.p)
					{
						hole[1] = HoleEdge{
							HoleNode{border0[indexIn0].p, border0[indexIn0].prevAfter},
							HoleNode{border0[mod(indexIn0 - 1, border0.size())].p, border0[mod(indexIn0 - 1, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 2, border0.size())].p, border0[mod(indexIn0 - 2, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 3, border0.size())].p, border0[mod(indexIn0 - 3, border0.size())].prevBefore}
						};
						hole[2] = HoleEdge{
							HoleNode{border0[mod(indexIn0 - 3, border0.size())].p, border0[mod(indexIn0 - 3, border0.size())].prevAfter},
							HoleNode{border0[mod(indexIn0 - 4, border0.size())].p, border0[mod(indexIn0 - 4, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 5, border0.size())].p, border0[mod(indexIn0 - 5, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 6, border0.size())].p, border0[mod(indexIn0 - 6, border0.size())].prevBefore}
						};
						return { hole };
					}
				}
				else if (foundIn0Minus3 != border0.end())
				{
					hole[0] = HoleEdge{
						HoleNode{border1[indexIn1].p, border1[indexIn1].prevAfter},
						HoleNode{border1[mod(indexIn1 - 1, border1.size())].p, border1[mod(indexIn1 - 1, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 - 2, border1.size())].p, border1[mod(indexIn1 - 2, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 - 3, border1.size())].p, border1[mod(indexIn1 - 3, border1.size())].prevBefore}
					};

					auto indexIn0 = std::distance(border0.cbegin(), foundIn0Minus3);
					if (border0[mod(indexIn0 + 6, border0.size())].p == point.p)
					{
						hole[1] = HoleEdge{
							HoleNode{border0[indexIn0].p, border0[indexIn0].prevBefore},
							HoleNode{border0[mod(indexIn0 + 1, border0.size())].p, border0[mod(indexIn0 + 1, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 2, border0.size())].p, border0[mod(indexIn0 + 2, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 3, border0.size())].p, border0[mod(indexIn0 + 3, border0.size())].prevAfter}
						};
						hole[2] = HoleEdge{
							HoleNode{border0[mod(indexIn0 + 3, border0.size())].p, border0[mod(indexIn0 + 3, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 4, border0.size())].p, border0[mod(indexIn0 + 4, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 5, border0.size())].p, border0[mod(indexIn0 + 5, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 6, border0.size())].p, border0[mod(indexIn0 + 6, border0.size())].prevAfter}
						};
						return { hole };
					}
					else if (border0[mod(indexIn0 - 6, border0.size())].p == point.p)
					{
						hole[1] = HoleEdge{
							HoleNode{border0[indexIn0].p, border0[indexIn0].prevAfter},
							HoleNode{border0[mod(indexIn0 - 1, border0.size())].p, border0[mod(indexIn0 - 1, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 2, border0.size())].p, border0[mod(indexIn0 - 2, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 3, border0.size())].p, border0[mod(indexIn0 - 3, border0.size())].prevBefore}
						};
						hole[2] = HoleEdge{
							HoleNode{border0[mod(indexIn0 - 3, border0.size())].p, border0[mod(indexIn0 - 3, border0.size())].prevAfter},
							HoleNode{border0[mod(indexIn0 - 4, border0.size())].p, border0[mod(indexIn0 - 4, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 5, border0.size())].p, border0[mod(indexIn0 - 5, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 6, border0.size())].p, border0[mod(indexIn0 - 6, border0.size())].prevBefore}
						};
						return { hole };
					}
				}
				// Second surface has two patches
				else if (foundIn0Plus6 != border0.end())
				{
					hole[0] = HoleEdge{
						HoleNode{border1[indexIn1].p, border1[indexIn1].prevBefore},
						HoleNode{border1[mod(indexIn1 + 1, border1.size())].p, border1[mod(indexIn1 + 1, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 + 2, border1.size())].p, border1[mod(indexIn1 + 2, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 + 3, border1.size())].p, border1[mod(indexIn1 + 3, border1.size())].prevAfter}
					};
					hole[1] = HoleEdge{
						HoleNode{border1[mod(indexIn1 + 3, border1.size())].p, border1[mod(indexIn1 + 3, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 + 4, border1.size())].p, border1[mod(indexIn1 + 4, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 + 5, border1.size())].p, border1[mod(indexIn1 + 5, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 + 6, border1.size())].p, border1[mod(indexIn1 + 6, border1.size())].prevAfter}
					};

					auto indexIn0 = std::distance(border0.cbegin(), foundIn0Plus6);
					if (border0[mod(indexIn0 + 3, border0.size())].p == point.p)
					{
						hole[2] = HoleEdge{
							HoleNode{border0[indexIn0].p, border0[indexIn0].prevBefore},
							HoleNode{border0[mod(indexIn0 + 1, border0.size())].p, border0[mod(indexIn0 + 1, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 2, border0.size())].p, border0[mod(indexIn0 + 2, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 3, border0.size())].p, border0[mod(indexIn0 + 3, border0.size())].prevAfter}
						};
						return { hole };
					}
					else if (border0[mod(indexIn0 - 3, border0.size())].p == point.p)
					{
						hole[2] = HoleEdge{
							HoleNode{border0[indexIn0].p, border0[indexIn0].prevAfter},
							HoleNode{border0[mod(indexIn0 - 1, border0.size())].p, border0[mod(indexIn0 - 1, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 2, border0.size())].p, border0[mod(indexIn0 - 2, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 3, border0.size())].p, border0[mod(indexIn0 - 3, border0.size())].prevBefore}
						};
						return { hole };
					}
				}
				else if (foundIn0Minus6 != border0.end())
				{
					hole[0] = HoleEdge{
						HoleNode{border1[indexIn1].p, border1[indexIn1].prevAfter},
						HoleNode{border1[mod(indexIn1 - 1, border1.size())].p, border1[mod(indexIn1 - 1, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 - 2, border1.size())].p, border1[mod(indexIn1 - 2, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 - 3, border1.size())].p, border1[mod(indexIn1 - 3, border1.size())].prevBefore}
					};
					hole[1] = HoleEdge{
						HoleNode{border1[mod(indexIn1 - 3, border1.size())].p, border1[mod(indexIn1 - 3, border1.size())].prevAfter},
						HoleNode{border1[mod(indexIn1 - 4, border1.size())].p, border1[mod(indexIn1 - 4, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 - 5, border1.size())].p, border1[mod(indexIn1 - 5, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 - 6, border1.size())].p, border1[mod(indexIn1 - 6, border1.size())].prevBefore}
					};

					auto indexIn0 = std::distance(border0.cbegin(), foundIn0Minus6);
					if (border0[mod(indexIn0 + 3, border0.size())].p == point.p)
					{
						hole[2] = HoleEdge{
							HoleNode{border0[indexIn0].p, border0[indexIn0].prevBefore},
							HoleNode{border0[mod(indexIn0 + 1, border0.size())].p, border0[mod(indexIn0 + 1, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 2, border0.size())].p, border0[mod(indexIn0 + 2, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 + 3, border0.size())].p, border0[mod(indexIn0 + 3, border0.size())].prevAfter}
						};
						return { hole };
					}
					else if (border0[mod(indexIn0 - 3, border0.size())].p == point.p)
					{
						hole[2] = HoleEdge{
							HoleNode{border0[indexIn0].p, border0[indexIn0].prevAfter},
							HoleNode{border0[mod(indexIn0 - 1, border0.size())].p, border0[mod(indexIn0 - 1, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 2, border0.size())].p, border0[mod(indexIn0 - 2, border0.size())].prevBefore},
							HoleNode{border0[mod(indexIn0 - 3, border0.size())].p, border0[mod(indexIn0 - 3, border0.size())].prevBefore}
						};
						return { hole };
					}
				}
			}

			return {};
		}
			
		case 3:
		{
			auto surface0 = dynamic_cast<C0Surface*>(surfaces[0]);
			auto surface1 = dynamic_cast<C0Surface*>(surfaces[1]);
			auto surface2 = dynamic_cast<C0Surface*>(surfaces[2]);

			auto&& border0 = surface0->getBorder();
			auto&& border1 = surface1->getBorder();
			auto&& border2 = surface2->getBorder();

			for (auto&& point : border0)
			{
				Hole hole;
				auto foundIn1 = findNode(point, border1);
				if (foundIn1 == border1.end())
					continue;
				int indexIn1 = std::distance(border1.cbegin(), foundIn1);

				auto foundIn2 = findNode(border1[mod(indexIn1 + 3, border1.size())], border2);
				int indexIn2 = std::distance(border2.cbegin(), foundIn2);
				if (foundIn2 != border2.end()) // case 1
				{
					hole[0] = HoleEdge{
						HoleNode{border1[indexIn1].p, border1[indexIn1].prevBefore},
						HoleNode{border1[mod(indexIn1 + 1, border1.size())].p, border1[mod(indexIn1 + 1, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 + 2, border1.size())].p, border1[mod(indexIn1 + 2, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 + 3, border1.size())].p, border1[mod(indexIn1 + 3, border1.size())].prevAfter}
					};
				}
				else
				{
					int index = mod(indexIn1 - 3, border1.size());
					foundIn2 = findNode(border1[mod(indexIn1 - 3, border1.size())], border2);
					indexIn2 = std::distance(border2.cbegin(), foundIn2);
					if (foundIn2 == border2.end())
						continue;
					// case 2
					hole[0] = HoleEdge{
						HoleNode{border1[indexIn1].p, border1[indexIn1].prevAfter},
						HoleNode{border1[mod(indexIn1 - 1, border1.size())].p, border1[mod(indexIn1 - 1, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 - 2, border1.size())].p, border1[mod(indexIn1 - 2, border1.size())].prevBefore},
						HoleNode{border1[mod(indexIn1 - 3, border1.size())].p, border1[mod(indexIn1 - 3, border1.size())].prevBefore}
					};
				}

				auto foundIn0 = findNode(border2[mod(indexIn2 + 3, border2.size())], border0);
				int indexIn0 = std::distance(border0.cbegin(), foundIn0);
				if (foundIn0 != border0.end()) //case 1
				{
					hole[1] = HoleEdge{
						HoleNode{border2[indexIn2].p, border2[indexIn2].prevBefore},
						HoleNode{border2[mod(indexIn2 + 1, border2.size())].p, border2[mod(indexIn2 + 1, border2.size())].prevBefore},
						HoleNode{border2[mod(indexIn2 + 2, border2.size())].p, border2[mod(indexIn2 + 2, border2.size())].prevBefore},
						HoleNode{border2[mod(indexIn2 + 3, border2.size())].p, border2[mod(indexIn2 + 3, border2.size())].prevAfter}
					};
				}
				else
				{
					foundIn0 = findNode(border2[mod(indexIn2 - 3, border2.size())], border0);
					indexIn0 = std::distance(border0.cbegin(), foundIn0);
					if (foundIn0 != border0.end()) // case 2
					{
						hole[1] = HoleEdge{
						HoleNode{border2[indexIn2].p, border2[indexIn2].prevAfter},
						HoleNode{border2[mod(indexIn2 - 1, border2.size())].p, border2[mod(indexIn2 - 1, border2.size())].prevBefore},
						HoleNode{border2[mod(indexIn2 - 2, border2.size())].p, border2[mod(indexIn2 - 2, border2.size())].prevBefore},
						HoleNode{border2[mod(indexIn2 - 3, border2.size())].p, border2[mod(indexIn2 - 3, border2.size())].prevBefore}
						};
					}
				}

				// final check
				if (border0[mod(indexIn0 + 3, border0.size())].p == point.p)
				{
					hole[2] = HoleEdge{
						HoleNode{border0[indexIn0].p, border0[indexIn0].prevBefore},
						HoleNode{border0[mod(indexIn0 + 1, border0.size())].p, border0[mod(indexIn0 + 1, border0.size())].prevBefore},
						HoleNode{border0[mod(indexIn0 + 2, border0.size())].p, border0[mod(indexIn0 + 2, border0.size())].prevBefore},
						HoleNode{border0[mod(indexIn0 + 3, border0.size())].p, border0[mod(indexIn0 + 3, border0.size())].prevAfter}
					};
					return { hole };
				}
				else if (border0[mod(indexIn0 - 3, border0.size())].p == point.p)
				{
					hole[2] = HoleEdge{
						HoleNode{border0[indexIn0].p, border0[indexIn0].prevAfter},
						HoleNode{border0[mod(indexIn0 - 1, border0.size())].p, border0[mod(indexIn0 - 1, border0.size())].prevBefore},
						HoleNode{border0[mod(indexIn0 - 2, border0.size())].p, border0[mod(indexIn0 - 2, border0.size())].prevBefore},
						HoleNode{border0[mod(indexIn0 - 3, border0.size())].p, border0[mod(indexIn0 - 3, border0.size())].prevBefore}
					};
					return { hole };
				}
			}
			return {};
		}

		default:
			return {};
		}
	}
}