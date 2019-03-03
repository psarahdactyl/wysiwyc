#!/usr/bin/env ruby

require 'nokogiri'

doc   = Nokogiri::XML(ARGF)
doc.traverse do |node|
  # determine if node needs to be split
  case node.name
  when "rect","polyline","polygon","path","circle","arc","ellipse"
    style = node.attribute("style")
    if not style.nil?
      hash = Hash[style.value.split(/\s*;\s*/).map { |s| s.split(/\s*:\s*/)}]
      # drawn objects: candidates for splitting
      if (not (hash["stroke"].nil? or hash["stroke"]=="none")) and 
          (not (hash["fill"].nil? or hash["fill"]=="none"))
        copy_node = node.clone
        copy_hash = hash.clone
        # copy will be put in front of this node; we want filled node to be
        # behind. So copy should store the strokes
        hash.delete("stroke")
        hash.delete("stroke-miterlimit")
        # Can't just delete since this will default to black fill
        #copy_hash.delete("fill")
        copy_hash["fill"] = "none"
        node.attribute("style").value = 
          hash.reduce(""){|all,(k,v)| all+k+":"+v+";"}
        copy_node.attribute("style").value = 
          copy_hash.reduce(""){|all,(k,v)| all+k+":"+v+";"}
        node.after(copy_node)
        node.after("\n")
      else
        # doesn't contain fill and stroke
      end
    else
      # no style apparently defaults to fill:black with no stroke...
    end
  when "text","comment","svg","document","line","g"
    # I know to ignore these.
  else
    STDERR.puts "<#{node.name}> ignored..."
  end
end

puts doc.to_xml
