#!/usr/bin/python
# coding:utf-8
import logging

logging.basicConfig(
    level=logging.WARNING,
    filename='./cpu-check.log',
    filemode='w',
    format=
    '%(asctime)s-%(filename)s[line:%(lineno)d]-%(levelname)s: %(message)s')

# use logging
core_id = input('Input the numberic part of your ID (such as 22040228): ')
print("Your file ysyx_" + core_id + ".v" + " will be check\n")

sig_port = [['input', 'clock'], ['input', 'reset'], ['input', 'io_interrupt'],
            ['input', 'io_master_awready'], ['output', 'io_master_awvalid'],
            ['output', '[31:0]', 'io_master_awaddr'],
            ['output', '[3:0]', 'io_master_awid'],
            ['output', '[7:0]', 'io_master_awlen'],
            ['output', '[2:0]', 'io_master_awsize'],
            ['output', '[1:0]', 'io_master_awburst'],
            ['input', 'io_master_wready'], ['output', 'io_master_wvalid'],
            ['output', '[63:0]', 'io_master_wdata'],
            ['output', '[7:0]', 'io_master_wstrb'],
            ['output', 'io_master_wlast'], ['output', 'io_master_bready'],
            ['input',
             'io_master_bvalid'], ['input', '[1:0]', 'io_master_bresp'],
            ['input', '[3:0]',
             'io_master_bid'], ['input', 'io_master_arready'],
            ['output', 'io_master_arvalid'],
            ['output', '[31:0]', 'io_master_araddr'],
            ['output', '[3:0]', 'io_master_arid'],
            ['output', '[7:0]', 'io_master_arlen'],
            ['output', '[2:0]', 'io_master_arsize'],
            ['output', '[1:0]', 'io_master_arburst'],
            ['output', 'io_master_rready'], ['input', 'io_master_rvalid'],
            ['input', '[1:0]',
             'io_master_rresp'], ['input', '[63:0]', 'io_master_rdata'],
            ['input', 'io_master_rlast'], ['input', '[3:0]', 'io_master_rid'],
            ['output', 'io_slave_awready'], ['input', 'io_slave_awvalid'],
            ['input', '[31:0]', 'io_slave_awaddr'],
            ['input', '[3:0]', 'io_slave_awid'],
            ['input', '[7:0]', 'io_slave_awlen'],
            ['input', '[2:0]', 'io_slave_awsize'],
            ['input', '[1:0]', 'io_slave_awburst'],
            ['output', 'io_slave_wready'], ['input', 'io_slave_wvalid'],
            ['input', '[63:0]', 'io_slave_wdata'],
            ['input', '[7:0]', 'io_slave_wstrb'], ['input', 'io_slave_wlast'],
            ['input', 'io_slave_bready'], ['output', 'io_slave_bvalid'],
            ['output', '[1:0]', 'io_slave_bresp'],
            ['output', '[3:0]',
             'io_slave_bid'], ['output', 'io_slave_arready'],
            ['input', 'io_slave_arvalid'],
            ['input', '[31:0]', 'io_slave_araddr'],
            ['input', '[3:0]', 'io_slave_arid'],
            ['input', '[7:0]', 'io_slave_arlen'],
            ['input', '[2:0]',
             'io_slave_arsize'], ['input', '[1:0]', 'io_slave_arburst'],
            ['input', 'io_slave_rready'], ['output', 'io_slave_rvalid'],
            ['output', '[1:0]', 'io_slave_rresp'],
            ['output', '[63:0]', 'io_slave_rdata'],
            ['output', 'io_slave_rlast'], ['output', '[3:0]', 'io_slave_rid']]

error = 0
top_mod_cnt = 0
core_file = 'ysyx_' + core_id + '.v'
for line in open(core_file, encoding='rt'):
    if "module " in line and "endmodule" not in line:
        if "module ysyx_" + core_id + "_" in line:
            pass
        elif "module ysyx_" + core_id in line and "module ysyx_22" + core_id + "_" not in line:
            #             print(line)
            top_mod_cnt = top_mod_cnt + 1
            pass  # 也许有学生所有模块都不加"_"连接，不规范，但是也能用，不过会影响之后的检查
        else:
            logging.error('module name is not right:')
            logging.error(line + '\n')  # 更改为写入到log文件中
            error = 1
            # print("no")# 更改为写入到log文件中，并注明错误原因
if top_mod_cnt > 1:
    error = 1  # 模块命名不规范，多个模块命名
if error == 1:
    print("Please check module name!!!\n")
    logging.error("Please check module name!!!")
# print(error)

flag = 0
cont = []
new_cont = []
for line in open(core_file, encoding='rt'):
    if 'module ysyx_' + core_id in line and 'module ysyx_' + core_id + '_' not in line:
        flag = 1

    if flag == 1:
        cont.append(line)
    if ")" in line and flag == 1:
        flag = 0
for j in cont[1:]:
    new_cont.append(
        j.replace(',', '').replace(')', '').replace(';', '').split())

for j in sig_port:
    if j in new_cont:
        print("error, you need to check code in : " + j[-1])
        logging.error("you need to check code in: " + j[-1])
        if error < 2:
            error = error + 2

info_ok = 'Your core is fine in module name and signal interface\n'
name_err = 'Your core has ERROR in module name\n'
io_err = 'Your core has ERROR in signal interface\n'
both_err = 'Your core has ERROR in module name and signal interface\n'
logging.info('\n\n\n\n\n\n')
if error == 0:
    print(info_ok)
    logging.critical(info_ok)
elif error == 1:
    print(name_err)
    logging.error(name_err)
elif error == 2:
    print(io_err)
    logging.error(io_err)
elif error == 3:
    print(both_err)
    logging.error(both_err)
