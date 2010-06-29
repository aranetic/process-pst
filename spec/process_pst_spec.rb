require File.join(File.dirname(__FILE__), 'spec_helper')

require 'assert2/xpath'

describe "process-pst" do
  include Test::Unit::Assertions # for assert2/xpath

  after do
    rm_rf(build_path("out"))
  end

  def loadfile
    build_path("out/edrm-loadfile.xml")
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
      _assert_xml(File.read(loadfile))
    end

    it "should succeed if passed valid arguments" do
      @result.should == true
    end

    it "should create a directory and edrm loadfile" do
      File.exist?(loadfile).should == true
    end

    it "should generate a valid EDRM loadfile" do
      xpath("/Root[@DataInterchangeType='Update']/Batch") do
        xpath("./Documents") { true }
        xpath("./Relationships") { true }
      end
    end

    it "should output metadata for each message and attachment in the PST" do
      xpath("//Document[@DocID='d0000001'][@DocType='Message']/Tags") do
        xpath("./Tag[@TagName='#Subject']" +
              "[@TagValue='Here is a sample message']" +
              "[@TagDataType='Text']") { true }
      end
      xpath("//Document[@DocID='d0000002'][@DocType='File']/Tags") do
        xpath("./Tag[@TagName='#FileName']" +
              "[@TagValue='leah_thumper.jpg']" +
              "[@TagDataType='Text']") { true }
      end      
    end
  end

  context "four_nesting_levels.pst" do
    before do
      process_pst("test_data/four_nesting_levels.pst", "out")
      _assert_xml(File.read(loadfile))       
    end

    it "should recurse through nested submessages" do
      subjects = ["Outermost message", "Middle message", "Innermost message"]
      subjects.each do |s|
        xpath("//Tag[@TagName='#Subject'][@TagValue='#{s}']") { true }
      end

      filenames = ["hello.txt"]
      filenames.each do |s|
        xpath("//Tag[@TagName='#FileName'][@TagValue='#{s}']") { true }
      end
    end

    it "should output attachment relationships" do
      relationships =
        [%w(d0000001 d0000002), %w(d0000002 d0000003), %w(d0000003 d0000004)]
      xpath("//Relationships") do
        relationships.each do |r|
          parent, child = r
          xpath("./Relationship[@Type='Attachment']" +
                "[@ParentDocID='#{parent}'][@ChildDocID='#{child}']") { true }
        end
      end
    end

    it "should output a text file for each email" do
      xpath("//Document/Files/File[@FileType='Text']") do
        xpath("./ExternalFile[@FileName='d0000001.txt']") { true }
      end
      path = build_path("out/d0000001.txt")
      File.exist?(path).should == true
    end

    it "should output the actual attached files" do
      xpath("//Document/Files/File[@FileType='Native']") do
        xpath("./ExternalFile" +
              "[@FileName='d0000004.txt']" +
              "[@FileSize='15']" +
              "[@Hash='78016cea74c298162366b9f86bfc3b16']") { true }
      end
      path = build_path("out/d0000004.txt")
      File.exist?(path).should == true
      File.size(path).should == 15
    end
  end
end
