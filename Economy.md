# Introduction #

Economy emulation is a huge stuff to deal. In game we want to make
  * supply and demand
  * technologies of industries
  * age of products
  * a lot of cargos (with subtypes: new milk, pasteurized milk, so on

# Details #
## Industry ##
![http://yuml.me/4187187a?.png](http://yuml.me/4187187a?.png)
```
#
#Simplified
[Material|+string name]
[Material]^[RawMaterial]
[Material]^[PreFabricated]
[Material]^[Product]
[Material]+-1>[MaterialType|+string units]
[MaterialType]-[note: Liquid bulk crates etc]
[TechnologyFraction|+uint amount|+uint quality]+-1>[Material],[TechnologyFraction]-[note: amount and quality are per each Technology]
[TechnologyFraction]-[note: quality - is contribution to final product of this fraction in procents]
[TechnologyOutput|+uint amount;+uint quality]+-1>[Material]
[TechnologyOutput]-[note: quality - is quality of product itself]
[Technology|list of TechnologyFraction inputs;list of TechnologyOutput output]-[note: each technology process will takes some material (fraction) as input and gives some material as output]
[Technology]+-*>[TechnologyFraction]
[Technology]+-*>[TechnologyOutput]
[Technology]-[note: Should be loaded from scripts definitions]
[Store]-[note: Is very useful object in game]
[Store|map of Material and amount]
[Industry|bitmask ProductionLines;OnTick(); OnDay(); OnMonth()]+-1>[IndustryType]
[IndustryType|bitmask LayoutType]+-1..2>[Technology]
[IndustryType]-[note: Can aggregate 1 or 2 technologies. E.g. chairs and beds. But final industry has bitmask of implemented technologies]
[Industry]+-1>[Store]
```

![http://yuml.me/7dfe1489?.png](http://yuml.me/7dfe1489?.png)
```
#Simplified
[Industry|Money RawPrice;Money ManufacturingPrice;Money SellPrice]
[Industry]+-1>[Store|list of CargoUnits Products]
[Store]+-0..*>[MaterialSupply|CargoUnits DemandLastMonth;CargoUnits DemandThisMonth;Money deliveryCost;OnRouteChange()]
[Store]+-0..*>[MaterialProvider]
[Industry]-[note: Support multiply routes for one material. Handle buy prices with care and math]
[Route]+-0..*>[Route|LengthUnits length]
[Route]^[MaterialSupply]
[Route]^[MaterialProvider]
[MaterialSupply]+-1>[Material]
[MaterialSupply]-[note: price of delivery should be estimated by median and then obtained from factual]
[MaterialProvider]+-1>[Material]
```