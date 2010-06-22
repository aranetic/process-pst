require File.join(File.dirname(__FILE__), 'spec_helper')

describe "process-pst" do
  after do
    FileUtils.rm_rf(build_path("out"))
  end

  it "should fail if passed invalid arguments" do
    process_pst("nosuch.pst", "out").should == false
  end

  context "sample1.pst" do
    before do
      @result = process_pst("pstsdk/test/sample1.pst", "out")
    end

    it "should succeed if passed valid arguments" do
      @result.should == true
    end

    it "should create a directory and edrm loadfile" do
      File.exist?(build_path("out/edrm-loadfile.xml")).should == true
    end
  end
end
