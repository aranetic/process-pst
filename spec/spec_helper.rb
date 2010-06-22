require 'rubygems'
require 'bundler'
require 'fileutils'

Bundler.setup(:default, :development)

require 'spec'
require 'spec/autorun'

$LOAD_PATH.unshift(File.dirname(__FILE__))

def build_path(path)
  "#{ENV['BUILD_ROOT']}/#{path}"
end

def source_path(path)
  "#{ENV['SOURCE_ROOT']}/#{path}"
end

def process_pst(pst, out_dir)
  system(build_path("process-pst"), source_path(pst), build_path(out_dir))
end

Spec::Runner.configure do |config|  
end
