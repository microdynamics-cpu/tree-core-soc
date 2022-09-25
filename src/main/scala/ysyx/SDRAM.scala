package ysyx

import chisel3._
import chisel3.experimental._

import freechips.rocketchip.amba.axi4._
import freechips.rocketchip.config.Parameters
import freechips.rocketchip.diplomacy._

class AXI4SDRAMIO extends Bundle {
    val awvalid_i = Input(Bool())
    val awaddr_i  = Input(UInt(32.W))
    val awid_i    = Input(UInt(4.W))
    val awlen_i   = Input(UInt(8.W))
    val awburst_i = Input(UInt(2.W))
    val wvalid_i  = Input(Bool())
    val wdata_i   = Input(UInt(32.W))
    val wstrb_i   = Input(UInt(4.W))
    val wlast_i   = Input(Bool())
    val bready_i  = Input(Bool())
    val arvalid_i = Input(Bool())
    val araddr_i  = Input(UInt(32.W))
    val arid_i    = Input(UInt(4.W))
    val arlen_i   = Input(UInt(8.W))
    val arburst_i = Input(UInt(2.W))
    val rready_i  = Input(Bool())
    val awready_o = Output(Bool())
    val wready_o  = Output(Bool())
    val bvalid_o  = Output(Bool())
    val bresp_o   = Output(UInt(2.W))
    val bid_o     = Output(UInt(4.W))
    val arready_o = Output(Bool())
    val rvalid_o  = Output(Bool())
    val rdata_o   = Output(UInt(32.W))
    val rresp_o   = Output(UInt(2.W))
    val rid_o     = Output(UInt(4.W))
    val rlast_o   = Output(Bool())
}

class SDRAMIO extends Bundle {
    val clk_o         = Output(Bool())
    val cke_o         = Output(Bool())
    val cs_o          = Output(Bool())
    val ras_o         = Output(Bool())
    val cas_o         = Output(Bool())
    val we_o          = Output(Bool())
    val dqm_o         = Output(UInt(2.W))
    val addr_o        = Output(UInt(13.W))
    val ba_o          = Output(UInt(2.W))
    val data_input_i  = Input(UInt(16.W))
    val data_output_o = Output(UInt(16.W))
    val data_out_en_o = Output(Bool())
}

class sdram_axi extends BlackBox (Map("SDRAM_MHZ" -> 100, "SDRAM_ADDR_W" -> 25,
                                  "SDRAM_COL_W" -> 10, "SDRAM_READ_LATENCY" -> 2)){
    val io = IO(new Bundle {
        val clk_i = Input(Clock())
        val rst_i = Input(Bool())
        val inport = Flipped(new AXI4SDRAMIO)
        // val inport = Flipped(new AXI4Bundle(CPUAXI4BundleParameters()))
        val sdram = new SDRAMIO
    })
}

class sdr extends BlackBox {
    val io = IO(new Bundle {
        val Clk   = Input(Bool())
        val Cke   = Input(Bool())
        val Cs_n  = Input(Bool())
        val Ras_n = Input(Bool())
        val Cas_n = Input(Bool())
        val We_n  = Input(Bool())
        val Addr  = Input(UInt(13.W))
        val Ba    = Input(UInt(2.W))
        val Dq    = Analog(16.W)
        val Dqm   = Input(UInt(2.W))
    })
}

class AXI4SDRAM(address: Seq[AddressSet])(implicit p: Parameters) extends LazyModule {
  val node = AXI4SlaveNode(Seq(AXI4SlavePortParameters(
    Seq(AXI4SlaveParameters(
      address       = address,
      executable    = false,
      supportsRead  = TransferSizes(1, 8),
      supportsWrite = TransferSizes.none)),
    beatBytes  = 8))) // for support same beatBytes xbar impl

    lazy val module = new LazyModuleImp(this) {
        val (in, _) = node.in(0)
        val io = IO(new SDRAMIO)
        val msdram = Module(new sdram_axi)
        msdram.io.clk_i := clock;
        msdram.io.rst_i := reset.asBool
        msdram.io.sdram <> io
        // msdram.io.inport.awvalid_i <> in.aw.valid
        msdram.io.inport <> in
        // msdram.io.inport.awaddr_i  <> 
        // msdram.io.inport.awid_i    <> 
        // msdram.io.inport.awlen_i   <> 
        // msdram.io.inport.awburst_i <> 
        // msdram.io.inport.wvalid_i  <> 
        // msdram.io.inport.wdata_i   <> 
        // msdram.io.inport.wstrb_i   <> 
        // msdram.io.inport.wlast_i   <> 
        // msdram.io.inport.bready_i  <> 
        // msdram.io.inport.arvalid_i <> 
        // msdram.io.inport.araddr_i  <> 
        // msdram.io.inport.arid_i    <> 
        // msdram.io.inport.arlen_i   <> 
        // msdram.io.inport.arburst_i <> 
        // msdram.io.inport.rready_i  <> 
        // msdram.io.inport.awready_o <> 
        // msdram.io.inport.wready_o  <> 
        // msdram.io.inport.bvalid_o  <> 
        // msdram.io.inport.bresp_o   <> 
        // msdram.io.inport.bid_o     <> 
        // msdram.io.inport.arready_o <> 
        // msdram.io.inport.rvalid_o  <> 
        // msdram.io.inport.rdata_o   <> 
        // msdram.io.inport.rresp_o   <> 
        // msdram.io.inport.rid_o     <> 
        // msdram.io.inport.rlast_o   <> 
    }
}