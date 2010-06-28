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
      xpath("/Root[@DataInterchangeType='Update']/Batch") do
        xpath("./Documents") { true }
        xpath("./Relationships") { true }
      end
    end

    it "should output metadata for each message and attachment in the PST" do
      _assert_xml(File.read(loadfile))
      xpath("//Document[@DocType='Message']/Tags") do
        xpath("./Tag[@TagName='#Subject']" +
              "[@TagValue='Here is a sample message']" +
              "[@TagDataType='Text']") { true }
      end
      xpath("//Document[@DocType='File']/Tags") do
        xpath("./Tag[@TagName='#FileName']" +
              "[@TagValue='leah_thumper.jpg']" +
              "[@TagDataType='Text']") { true }
      end      
    end

    # it should recurse through nested submessages
  end
end
