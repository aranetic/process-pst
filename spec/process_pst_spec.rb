require File.join(File.dirname(__FILE__), 'spec_helper')

describe "process-pst" do
  it "should fail if passed invalid arguments" do
    process_pst("nosuch.pst", "out").should == false
  end

  it "should succeed if passed valid arguments" do
    process_pst("pstsdk/test/sample1.pst", "sample1-out").should == true
  end

  it "should create a directory and edrm loadfile" do
    process_pst("pstsdk/test/sample1.pst", "sample1-out")
    File.exist?(build_path("sample1-out/edrm-loadfile.xml")).should == true
  end
end
