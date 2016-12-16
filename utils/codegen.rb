#!/usr/bin/env ruby

def gen(workset_min, workset_max, element_min, element_max)
  File.open('src/gen.h', 'w') do |f|
    f.write "static void run(std::ostringstream& out, float speed) {\n"
    element = element_min
    while element <= element_max do
      (workset_min..workset_max).each do |workset|
        f.write "  run<#{workset}, #{element}, Linear>(out, speed);\n"
        f.write "  run<#{workset}, #{element}, Reverse>(out, speed);\n"
        f.write "  run<#{workset}, #{element}, Random>(out, speed);\n"
      end
      element = element * 2
    end
    f.write "}\n"
  end
end

gen(ARGV[0].to_i, ARGV[1].to_i, ARGV[2].to_i, ARGV[3].to_i)
