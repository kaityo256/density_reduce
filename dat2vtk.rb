require 'pathname'

if ARGV.size == 0
  puts "usage: ruby dat2vtk.rb datfile"
  exit
end
inputfile = ARGV[0]

data = IO.binread(inputfile)
density = data.unpack("d*")
dim = 51

outputfile = Pathname(inputfile).sub_ext(".vtk").to_s

f = open(outputfile,"w")
f.puts <<"EOS"
# vtk DataFile Version 1.0
density
ASCII
DATASET STRUCTURED_POINTS
DIMENSIONS #{dim} #{dim} #{dim}
ORIGIN 0.0 0.0 0.0
SPACING 1.0 1.0 1.0
POINT_DATA #{dim**3}
SCALARS intensity float
LOOKUP_TABLE default
EOS
density.each do |d|
  f.puts d
end

puts "#{inputfile} -> #{outputfile}"
