# mongo mapnik plugin

This is a bare bones connector to mongodb point data in the form of a mapnik C++ plugin.


    <?xml version="1.0" encoding="utf-8"?>
    <Map srs="+init=epsg:4326" background-color="white">
        <Style name="style">
            <Rule>
                <PointSymbolizer />
                <TextSymbolizer name="[key]" face_name="DejaVu Sans Book" size="10" dx="5" dy="5"/>
            </Rule>
        </Style>
        <Layer name="test" srs="+init=epsg:4326">
            <StyleName>style</StyleName>
            <Datasource>
                <Parameter name="type">mongo</Parameter>
            </Datasource>
        </Layer>
    </Map>


Or used in python like:

    from mapnik2 import *
    m = Map(600,400)
    m.background = Color('white')
    s = Style()
    r = Rule()
    r.symbols.append(PointSymbolizer())
    t = TextSymbolizer(Expression("[key]"),"DejaVu Sans Book",10,Color('black'))
    t.displacement(15,15)
    r.symbols.append(t)
    s.rules.append(r)
    m.append_style('style',s)
    ds = Datasource(type="mongo")
    l = Layer('test')
    l.styles.append('style')
    l.datasource = ds
    m.layers.append(l)
    m.zoom_all()
    render_to_file(m,'test.png')
