L = 100
N = 100000
a = []
C = L*0.5
R = L*0.4
loop do
  theta = 2.0*Math::PI*rand
  r = rand**(1.0/3.0)
  z = rand*2.0 - 1.0
  x = C + R*r*Math::sqrt(1-z**2)*Math::cos(theta)
  y = C + R*r*Math::sqrt(1-z**2)*Math::sin(theta)
  z = C + R*r*z
  a.push x,y,z
  break if a.size >= N*3
end

IO.binwrite("test.dump", a.pack("d*"))
puts "Saved as test.dump"
