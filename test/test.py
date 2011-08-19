import mapnik2 as mapnik

mapnik.register_plugins('../') # get mongo.input

m = mapnik.Map(600,400)
m.background = mapnik.Color('white')
s = mapnik.Style()
r = mapnik.Rule()
r.symbols.append(mapnik.PointSymbolizer())
t = mapnik.TextSymbolizer(mapnik.Expression("[key]"),"DejaVu Sans Book",10,mapnik.Color('black'))
t.displacement = (15,15)
r.symbols.append(t)
s.rules.append(r)
m.append_style('style',s)
ds = mapnik.Datasource(type="mongo")
l = mapnik.Layer('test')
l.styles.append('style')
l.datasource = ds
m.layers.append(l)
m.zoom_all()
mapnik.render_to_file(m,'test.png')

