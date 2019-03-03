# Exploder tools for fills and strokes

Running

    ruby explode.rb filename.svg >flattened.svg

will read all svg objects in `filename.svg` and will _split_ any object with
both a fill and a stroke into two objects: one with _just_ a fill and the other
with _just_ the stroke. All ordering, groupings, other attributes, and other
objects will be left in tact.

## Dependencies

Depends on the [Nokogiri](https://nokogiri.org) xml parsing library. This can be
installed using rubygems. For example:


    sudo gem install nokogiri


> ## Legacy Python Version
> 
> Usage
> ```
> python exploder_strokes_fills.py filename.svg
> ```
> 
> where:
> * ```filename``` is the input SVG file (Currently only supports Adobe
>   Illustrator SVG 1.0 Files **with CSS Properties: "Style Attributes"**)
> 
> The file is saved in the present working directory as ```filename-exploded.svg```
