require File.join(File.dirname(__FILE__), 'spec_helper')

require 'assert2/xpath'

describe "process-pst" do
  include Test::Unit::Assertions # for assert2/xpath

  after do
    rm_rf(build_path("out"))
  end

  it "should fail if passed invalid arguments" do
    process_pst("nosuch.pst", "out").should == false
  end

  it "should fail if the output directory exists" do
    mkdir_p(build_path("out"))
    process_pst("pstsdk/test/sample1.pst", "out").should == false
  end

  context "sample1.pst" do
    before do
      @result = process_pst("pstsdk/test/sample1.pst", "out")
    end

    def loadfile
      build_path("out/edrm-loadfile.xml")
    end

    it "should succeed if passed valid arguments" do
      @result.should == true
    end

    it "should create a directory and edrm loadfile" do
      File.exist?(loadfile).should == true
    end

    it "should generate a valid EDRM loadfile" do
      _assert_xml(File.read(loadfile))
      assert do
        xpath("/Root[@DataInterchangeType='Update']/Batch/Documents")
      end
    end
  end
end
