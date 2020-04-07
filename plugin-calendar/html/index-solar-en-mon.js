/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

// LunarCalendar.js currently supports this range
var year_range = { 'low': 1891, 'high': 2100 };
var weekdays = ['Sun','Mon', 'Tue', 'Wed', 'Thur', 'Fri', 'Sat'];
var calendar = null;
var today = new Date();
var highlight_day = today.getDate(); // 鼠标选中的日期值，范围1-31
var year_selector = null;
var current_ui= null;
var month_selector = null;
var month_button = null;
var year_button = null;
var hl_script = null;
var year = null;
var month = null;
var scrollUp_count = 0;
var scrollDown_count = 0;
var selected_date_div = null;
var NeedChangeCurrentTime = 1;

var div_range = {
    year: {
        x_min: 0,
        x_max: 0,
        y_min: 0,
        y_max: 0
    },
    month: {
        x_min: 0,
        x_max: 0,
        y_min: 0,
        y_max: 0
    },
    holiday: {
        x_min: 0,
        x_max: 0,
        y_min: 0,
        y_max: 0
    }
};

 function addZero(v) {
    if ( v < 10 ) {
         return '0' + v;
    } else {
        return '' + v;
        }
  }

function getTime(d) {
    return [
        addZero(d.getHours()),
        addZero(d.getMinutes()),
        addZero(d.getSeconds())
        ].join(':');
  }

function convertToNormal()
    {
        document.getElementById('month_div').className = 'hidden_div';
        document.getElementById('year_div').className = 'hidden_div';
        document.getElementById('calendar_table').style.display = "";
    }

function updateTime() {
        var timer = null;
        var today_time = new Date();
        datetime_container.children[0].innerHTML = getTime(today_time);
        timer = setInterval(function() {
        var today_time = new Date();
        datetime_container.children[0].innerHTML = getTime(today_time);
        }, 500);
  }

function update_month_ui(mode)
{
    var li = document.getElementById('month_div');
    for(index = 0; index < 16; index++)
    {
        li.children[0].children[index].removeEventListener('click', new_month_selected);
    }
    var list = null;
    var bind_click_position = null;
    var bind_click_count  = 0;
    if(scrollUp_count %3 ===1)//mode 2 &&the second ui  for month
   {
        if(mode === 0)
        {
            list = [9,10,11,12,1,2,3,4,5,6,7,8,9,10,11,12];//show diff month by scroll  mouse
            bind_click_position = 4;
        }
        else
        {
            list = [1,2,3,4,5,6,7,8,9,10,11,12,1,2,3,4]; //show diff month by scroll  mouse
            bind_click_position = 0;
        }
   } 
   else if(scrollUp_count %3 ===2)//mode 3 && the third ui for month
   {
       if(mode ===0)//roll up
       {
            list = [5,6,7,8,9,10,11,12,1,2,3,4,5,6,7,8];   //show diff month by scroll  mouse
            bind_click_position = 0;//show prev years month,and year need  -1 from this time
            year--;
            year_selector.value = year;
            selected_date_div.innerHTML = year_selector.value +'.' +month_selector.value;
       }
       else//roll down
       {
            list = [9,10,11,12,1,2,3,4,5,6,7,8,9,10,11,12];//show diff month by scroll  mouse
            bind_click_position = 4;  
       }

   }
   else//mode 3 && the first ui for month
   {
        if(mode === 0)
        {
            list = [1,2,3,4,5,6,7,8,9,10,11,12,1,2,3,4]; //show diff month by scroll  mouse
            bind_click_position = 0;
            // year--;
            // year_selector.value = year+'年';
        }
        else
        {
            list = [5,6,7,8,9,10,11,12,1,2,3,4,5,6,7,8];   //show diff month by scroll  mouse
            bind_click_position = 8;//show prev years month,and year need  -1 from this time
            year++;
            year_selector.value = year;
            selected_date_div.innerHTML = year_selector.value +'.'+ month_selector.value;
        }
   }
   if(mode == 1)
   {
       if(scrollUp_count > 0)
       {
        scrollUp_count--;
       }
       else
       {
        scrollUp_count = 2;
       }
   }
   for(var index = 0; index <16; index++)
   {
    //   li.children[0].children[index].innerHTML = '<br />';
       //li.children[0].children[index].innerHTML = '<br />'+ list[index]+ '月';
       li.children[0].children[index].innerHTML ='<span class="year_month_grid">'+ list[index] + '</span>';
    //    li.children[0].children[index].innerHTML =  list[index]+ '月';
    //    document.getElementsByTagName('span').style="float:left;width:250px;background:#6C3;";

       if(index >= bind_click_position)
       {
            if(scrollUp_count%3 ===2)
            {
                if(bind_click_count < 8)
                {
                    li.children[0].children[index].style.color  = "#FFFFFFFF";
                    li.children[0].children[index].addEventListener('click', new_month_selected);
                    bind_click_count = bind_click_count + 1;
                }
                else
                {
                    li.children[0].children[index].style.color  = "#FFFFFF33";
                }
               continue;
            }    

            if(bind_click_count < 12) // max bind _count of month click is no more than 12
            {
                li.children[0].children[index].style.color  = "#FFFFFFFF";
                li.children[0].children[index].addEventListener('click', new_month_selected);
                bind_click_count = bind_click_count + 1;
            } 
            else
            {
                li.children[0].children[index].style.color  = "#FFFFFF33";
            }  
       }
       else
       {
            li.children[0].children[index].style.color  = "#FFFFFF33";
       }
   }
}

function update_year_month_ui()
{
    var li = document.getElementById('year_div');
    for (var index =  0;  index < 16;  index++) {
        // li.children[0].children[index].innerHTML = '<br />'; 
        var  curretYear = year + index;
        li.children[0].children[index].innerHTML=  '<span class="year_month_grid">'+curretYear +'</span>' ;
        //li.children[0].children[index].innerHTML= '<br />'+curretYear+ '年';
        // if(index === 0)
        // {
        //     li.children[0].children[index].style.backgroundColor = "#2b87a8";
        // }
        li.children[0].children[index].addEventListener('click', new_month_selected); // new year implies new month
        // year_list.appendChild(li);
        // if (index === year) {
        //     year_selector.value = index + '年';
        // }
    }
    li = document.getElementById('month_div');

    for (var index = 0; index < 16; index++) {
        li.children[0].children[index].removeEventListener('click', new_month_selected);
        li.children[0].children[index].style.color  = "#FFFFFFFF";
        
        if(index >=12)
        {
            var newIndex =  index -12 + 1;
            li.children[0].children[index].style.color  = "#FFFFFF33";
            //li.children[0].children[index].innerHTML = '<br />' +  new_index + '月';
            li.children[0].children[index].innerHTML=  '<span class="year_month_grid">'+newIndex +'</span>' ;
            //li.children[0].children[index].innerHTML = "<font color=rgba(255,255,255,0.2)>1月</font>";
        }
        else
        {
            var newIndex =  index + 1;
            //li.children[0].children[index].innerHTML ='<br />' + newIndex+ '月';
            li.children[0].children[index].innerHTML ='<span class="year_month_grid">'+ newIndex + '</span>' ;
        }
        if(index < 12)
        {
            li.children[0].children[index].addEventListener('click', new_month_selected);
        }
        // month_list.appendChild(li);
        if (index === month + 1) {
            month_selector.value = index;
        }
    }
}

function updateUi()
{
    year_selector.value = year;
    update_year_month_ui();
    // var li = document.getElementById('year_div');
    // for (var index =  0;  index < 16;  index++) {
    //     li.children[0].children[index].innerHTML= year + index + '年';
    //     li.children[0].children[index].addEventListener('click', new_month_selected); // new year implies new month
    //     // year_list.appendChild(li);
    //     // if (index === year) {
    //     //     year_selector.value = index + '年';
    //     // }
    // }
    // li = document.getElementById('month_div');
    // for (var index = 0; index < 16; index++) {
    //     if(index >=12)
    //     {
    //         var new_index =  index -12;
    //         li.children[0].children[index].innerHTML = new_index +1+ '月';
    //     }
    //     else
    //     {
    //         li.children[0].children[index].innerHTML = index +1+ '月';
    //     }
    //     li.children[0].children[index].addEventListener('click', new_month_selected);
    //     // month_list.appendChild(li);
    //     if (index === month + 1) {
    //         month_selector.value = index + '月';
    //     }
    // }
}

function scroll_div(event)
{
    var e = event||window.event;
    if(e.wheelDelta > 0)
    {
        if(this.id === 'year_div')
        {
            year = year - 4;
        }
        else if(this.id === 'month_div')
        {
            scrollUp_count++;
            update_month_ui(0);
            return;
        }
    }
    else
    {       
        if(this.id === 'year_div')
        {
           year = year + 4;
        }
        else if(this.id === 'month_div')
        {
            scrollDown_count++;
            update_month_ui(1);
            return;
        }
    }
    updateUi();
}

function update_yiji_area() {
    "use strict";
    var year = parseInt(year_selector.value, 10);
    var month = parseInt(month_selector.value, 10);

    var hl_table = document.getElementById('hl_table');
    var current_row;
    var current_cell;

    if (year !== parseInt(hl_script.id, 10)) {
        load_hl_script(year);
        return;
    }

    if (typeof HuangLi['y' + year] === 'undefined') {
        for (var row = 0; row < 2; row++) {
            if (hl_table.rows.length === row) {
                current_row = hl_table.insertRow(row);
            } else {
                current_row = hl_table.rows[row];
            }

            for (var column = 1; column < 5; column++) {
                if (current_row.cells.length === column) {
                    current_cell = current_row.insertCell(column);
                } else {
                    current_cell = current_row.cells[column];
                }

                if (row === 1) {
                    current_cell.innerHTML = '';
                } else {
                    current_cell.innerHTML = '';
                }
            }
        }
    }

    var day = highlight_day;
    var month_str = month.toString();
    var day_str = day.toString();
    if (month <= 9) {
        month_str = '0' + month;
    }
    if (day <= 9) {
        day_str = '0' + day;
    }

    var yi_str = HuangLi['y' + year]['d' + month_str + day_str]['y'];
    var ji_str = HuangLi['y' + year]['d' + month_str + day_str]['j'];
    var hl_yi_data = yi_str.split('.');
    var hl_ji_data = ji_str.split('.');

    for (var row = 0; row < 2; row++) {
        if (hl_table.rows.length === row) {
            current_row = hl_table.insertRow(row);
        } else {
            current_row = hl_table.rows[row];
        }

        for (var column = 1; column < 5; column++) {
            if (current_row.cells.length === column) {
                current_cell = current_row.insertCell(column);
            } else {
                current_cell = current_row.cells[column];
            }
                if(0 == row )
                {
                    if(hl_yi_data[column-1])
                    {
                        current_cell.innerHTML = hl_yi_data[column-1]; 
                    }
                    else
                    {
                        current_cell.innerHTML =  ''; 
                    }
                    
                }
                else
                {
                    if(hl_ji_data[column-1])
                    {
                        current_cell.innerHTML = hl_ji_data[column-1];
                    }
                    else
                    {
                        current_cell.innerHTML =''; 
                    }       
                }
        }
    }
}

function load_hl_script(year) {
    "use strict";

    if (hl_script) {
        document.body.removeChild(hl_script);
        hl_script = null;
    }

    hl_script = document.createElement("SCRIPT");
    hl_script.type = "text/javascript";
    hl_script.src = "hl/hl" + year + '.js';
    hl_script.id = year;
    hl_script.onload = function() {
        update_yiji_area();
    };
    document.body.appendChild(hl_script);
}

window.onload = function () {

    "use strict";
    load_hl_script(today.getFullYear());
    // var year_list = document.getElementById('year_list');
    // var month_list = document.getElementById('month_list');

    year = today.getFullYear();
    month = today.getMonth();
    var real_month = month + 1;
    selected_date_div = document.getElementById('selected_date_div');
    year_selector = document.createElement('year_selector');
    month_selector = document.createElement('month_selector');
    year_selector.value = year;
    month_selector.value = real_month;
    selected_date_div.innerHTML = year_selector.value +'.'+month_selector.value;
    // year_selector = document.getElementById('year_selector');
    // // year_selector.addEventListener('click', popup_div);

    // month_selector = document.getElementById('month_selector');
    // month_selector.addEventListener('click', popup_div);
    //end 
    // alert("begin");
    year_button = document.getElementById('year_button');
    year_button.addEventListener('click', popup_div);
    // alert("end");
    month_button= document.getElementById('month_button');
    month_button.addEventListener('click', popup_div);
    document.getElementById('year_div').addEventListener('mousewheel',scroll_div);
    document.getElementById('month_div').addEventListener('mousewheel',scroll_div);
    document.addEventListener('click', function(event) {
    });

    updateUi();
    var goto_arrows = document.getElementsByTagName('input');
    var n_months = (year_range['high'] - year_range['low'] + 1) * 12;
    for (var index = 0; index < goto_arrows.length; index++) {
        goto_arrows[index].addEventListener('click', function() {

            var year = parseInt(year_selector.value);
            var month = parseInt(month_selector.value);
            //page  the year ui
            if(document.getElementById('year_div').className ==='visible_div')
            {
                 var li = document.getElementById('year_div');
                if(this.id === 'go_prev_month')
                {
                    year = year -16;
                    year_selector.value = year;
                    // selected_date_div.innerHTML = year_selector.value + month_selector.value;
                    for (var index =  0;  index < 16;  index++) {
                        
                        // li.children[0].children[index].innerHTML = '<br />';
                        var currentYear = year + index;
                        //li.children[0].children[index].innerHTML= '<br />' + curretYear + '年';
                        li.children[0].children[index].innerHTML ='<span class="year_month_grid">'+ currentYear + '</span>';
                        li.children[0].children[index].addEventListener('click', new_month_selected); // new year implies new month
                    }
                }
                else if(this.id === 'go_next_month')
                {
                    year = year + 16;
                    year_selector.value = year;
                    // selected_date_div.innerHTML = year_selector.value + month_selector.value;
                    for (var index =  0;  index < 16;  index++) {
                        
                        // li.children[0].children[index].innerHTML = '<br />';
                        var currentYear = year + index;
                        //li.children[0].children[index].innerHTML=  '<br />'+ curretYear + '年';
                        li.children[0].children[index].innerHTML ='<span class="year_month_grid">'+ currentYear + '</span>';
                        li.children[0].children[index].addEventListener('click', new_month_selected); // new year implies new month
                    }
                }
                return;
            }
            else if(document.getElementById('month_div').className ==='visible_div')//page the month ui
            {
                if(this.id === 'go_prev_month')
                {
                    year --;
                    year_selector.value = year;
                    selected_date_div.innerHTML = year_selector.value + '.' +month_selector.value;
                }
                else if(this.id === 'go_next_month')
                {
                    year++;
                    year_selector.value = year;
                    selected_date_div.innerHTML = year_selector.value + '.' + month_selector.value;
                }     
                return;
            }
            // var year = parseInt(year_selector.value);
            // var month = parseInt(month_selector.value);
            var month_offset = (year - year_range['low']) * 12 + month - 1; // [0, n_months - 1]

            if (this.id === 'go_prev_year') {
                month_offset -= 12;
            } else if (this.id === 'go_next_year') {
                month_offset += 12;
            } else if (this.id === 'go_prev_month') {
                month_offset -= 1;
            } else if (this.id === 'go_next_month') {
                month_offset += 1;
            } else {
                return;
            }

            if (month_offset < 0 || month_offset > n_months - 1) {
                return;
            }

            year_selector.value = Math.floor(month_offset / 12) + year_range['low'];
            month_selector.value = month_offset % 12 === 0 ? 1 : month_offset % 12 + 1;
            selected_date_div.innerHTML = year_selector.value + '.' +month_selector.value;
            create_page(parseInt(year_selector.value), parseInt(month_selector.value));
        });
    }
    var holidays = ['元旦节', '春节', '清明节', '劳动节', '端午节', '中秋节', '国庆节'];
    var holiday_list = document.getElementById('holiday_list');
    for (var index = 0; index < holidays.length; index++) {
        var li = document.createElement('LI');
        li.innerHTML = holidays[index];
        li.addEventListener('click', go_to_holiday);
        holiday_list.appendChild(li);
    }
    //var holiday_button = document.getElementById('holiday_button');
    //holiday_button.addEventListener('click', popup_div);

    var today_button = document.getElementById('today_button');
    today_button.addEventListener('click', function() {
        year_selector.value = today.getFullYear();
        month_selector.value = today.getMonth() + 1;
        selected_date_div.innerHTML = year_selector.value + '.' + month_selector.value;
        highlight_day = today.getDate();
        convertToNormal();
        year = today.getFullYear();
        month =  today.getMonth();
        create_page(today.getFullYear(), today.getMonth() + 1);
        update_year_month_ui();
	var header_id=document.getElementById("header");
	var header_color=header_id.style.background;
	var x=document.getElementsByClassName("day_today");
	var i;
	if (header_color == "rgb(0, 0, 0)"){
	    for (i = 0; i < x.length; i++) {
		x[i].style.backgroundColor = "#3593b5";
	    }
	}
	else{
	    for (i = 0; i < x.length; i++) {
		x[i].style.backgroundColor = header_color;
	    }
	}
    });
    
    calendar = document.getElementById('calendar_table');
    create_page(parseInt(year_selector.value), parseInt(month_selector.value));
}

function create_page(year, month) {

    if (year < year_range['low'] || year > year_range['high'])
        return;
    var month_stuff = LunarCalendar.calendar(year, month, true,1);
    highlight_day = highlight_day > month_stuff['monthDays'] ? month_stuff['monthDays'] : highlight_day;

    var current_row = null;
    var current_cell = null;
    for (var row = 1; row < 7; row++) {
        if (calendar.rows.length === row) {
            current_row = calendar.insertRow(row);
        } else {
            current_row = calendar.rows[row];
        }

        for (var column = 0; column < 7; column++) {
            if (current_row.cells.length === column) {
                current_cell = current_row.insertCell(column);
                current_cell.addEventListener('click', function() {
                    highlight_day = parseInt(this.children[0].innerHTML);

                    if (this.className === 'day_other_month') {
                        return;
                    }

                    create_page(parseInt(year_selector.value), parseInt(month_selector.value));
                });
            } else {
                current_cell = current_row.cells[column];
            }

            var index = (row - 1) * 7 + column; // [0, 7 * 6 - 1]
            /*
             * 注意判断顺序
             * 1. 表格开头/结尾的非本月日期的单元格样式与其他单元格无关，需要最先判断
             * 2. 其次是‘今天’的单元格样式
             * 3. 再次是当前鼠标点击选中的单元格
             * 4. 再次是属于周末的单元格
             * 5. 最后是其他普通单元格
             */
            if ((index < (month_stuff['firstDay'] -1) && month_stuff['firstDay'] != 0)//每周从周一开始,当月第一天不是星期天,上个月的日期
                ||(index < 6 && month_stuff['firstDay'] === 0)//第一天是星期天
                || (month_stuff['firstDay']===0 && index >= (month_stuff['monthDays'] + 6))//第一天是星期天下月的日期
                ||( month_stuff['firstDay'] != 0 && index >= month_stuff['firstDay'] + month_stuff['monthDays']-1)) //第一天不是星期天下月日期
            {
                current_cell.className = 'day_other_month';
            } else if (today.getDate() === month_stuff['monthData'][index]['day'] &&
                       today.getMonth() === month - 1 &&
                       today.getFullYear() === year) {
                current_cell.className = 'day_today';
            } else if (index === highlight_day + month_stuff['firstDay'] - 1) {
                current_cell.className = 'day_highlight';
            } else if (column === 0 || column === 6) {
                current_cell.className = 'day_weekend';
            } else {
                current_cell.className = 'day_this_month';
            }

            var lunar_day;
            if (month_stuff['monthData'][index]['lunarFestival']) {
                lunar_day = month_stuff['monthData'][index]['lunarFestival'];
//            } else if (month_stuff['monthData'][index]['solarFestival']) {
//                lunar_day = month_stuff['monthData'][index]['solarFestival'];
            } else {
                lunar_day = month_stuff['monthData'][index]['lunarDayName'];
            }

            var worktime = null;
            if (month_stuff['monthData'][index]['worktime'] === 2) {
                worktime = document.createElement("SPAN");
                worktime.className = 'worktime2';
                worktime.innerHTML =  '<tr style="background: red" align="left"><td> <img src="images/xiuxi.png" align="left" width = "14px" height = "16px"></td> </tr>';
            } else if (month_stuff['monthData'][index]['worktime'] === 1) {
                worktime = document.createElement("SPAN");
                worktime.className = 'worktime1';
                worktime.innerHTML =  '<tr style="background: red" align="left"><td> <img src="images/shangban.png" align="left" width = "16px" height = "16px"></td> </tr>';
            } else {

            }
                 /*myworktime.innerHTML =   '<span class="solar_part">' +
                                     month_stuff['monthData'][index]['day'] +
                                     '</span>' +
                                     '<br />' +
                                     '<span class="lunar_part">' +
                                     lunar_day +
                                     '</span>';*/
            // if (worktime && current_cell.className !== 'day_other_month') {
            //     //current_cell.appendChild(worktime);
            //     // <td><div id="aa"></div></td>
            //     //  document.getElementById('aa').innerHTML = worktime.innerHTML;
            //      current_cell.innerHTML = worktime.innerHTML+
            //                         //   '<br />'+ 
            //                         '<span class="solar_part" > ' +
            //                          month_stuff['monthData'][index]['day'] +
            //                          '</span>' +
            //                          '<br />' +
            //                          '<span class="lunar_part">' +
            //                          lunar_day +
            //                          '</span>';
            //     // current_cell.innerHTML =  '<tr style="background: green"><td>1</td><td>2</td><td>3</td></tr>'
            //     // +'<br />' + '<tr style="background: green"><td>4</td><td>5</td><td>6</td></tr>'+
            //     // '<br />' +'<tr style="background: green"><td>7</td><td>8</td><td>9</td></tr>';
            // }
            // else
			// {
            //      current_cell.innerHTML =   '<span class="solar_part">' +
            //                          month_stuff['monthData'][index]['day'] +
            //                          '</span>' +
            //                          '<br />' +
            //                          '<span class="lunar_part">' +
            //                          lunar_day +
            //                          '</span>';
            // }
            current_cell.innerHTML =   '<span class="solar_part">' +
            month_stuff['monthData'][index]['day'] +
            '</span>'
        }
    }

    update_right_pane(year, month, highlight_day);

    month_stuff = null;

    if (header_color == "rgb(0, 0, 0)"){
        var day_this_month_len=document.getElementsByClassName('day_this_month').length;
        for (var i=0; i<day_this_month_len; i++){
	    document.getElementsByClassName('day_this_month')[i].getElementsByClassName('solar_part')[0].style.color='#ffffff';
            document.getElementsByClassName('day_this_month')[i].getElementsByClassName('lunar_part ')[0].style.color='#aaaaaa';
        }	
        var day_other_month_len=document.getElementsByClassName('day_other_month').length;
        for (var i=0; i<day_other_month_len; i++){
            document.getElementsByClassName('day_other_month')[i].getElementsByClassName('solar_part')[0].style.color='#777777';
            document.getElementsByClassName('day_other_month')[i].getElementsByClassName('lunar_part ')[0].style.color='#777777';
        }  
        var day_today_len=document.getElementsByClassName('day_today').length;
        for (var i=0; i<day_today_len; i++){
            document.getElementsByClassName('day_today')[i].getElementsByClassName('solar_part')[0].style.color='#ffffff';
            document.getElementsByClassName('day_today')[i].getElementsByClassName('lunar_part ')[0].style.color='#ffffff';
        }
	   
    }
}

function new_month_selected() {
    if (this.parentNode.className=== 'show_years') {
        var str = this.innerHTML.replace("<span class=\"year_month_grid\">","");
        year_selector.value = str.replace("</span>","");
        document.getElementById('year_div').className = 'hidden_div';
        calendar.style.display = "";
    } else if (this.parentNode.className=== 'show_months') {
        var str = this.innerHTML.replace("<span class=\"year_month_grid\">","");
        month_selector.value = str.replace("</span>","");
        document.getElementById('month_div').className = 'hidden_div';
        calendar.style.display = "";
    }
    selected_date_div.innerHTML = year_selector.value + '.' + month_selector.value;
    create_page(parseInt(year_selector.value), parseInt(month_selector.value));
}

function popup_div(event) {
    var x = event.clientX - event.offsetX;
    var y = event.clientY - event.offsetY;
    var div;
    // TODO
    var width = 64;
    var height = 20;
    if (this.id === 'year_button') {
        div = document.getElementById('year_div');

        div_range.year.x_min = x;
        div_range.year.x_max = x + width;
        div_range.year.y_min = y;
        div_range.year.y_max = y + height;

        if (div.className === 'hidden_div') {
            div.className = 'visible_div';
            document.getElementById('month_div').className = 'hidden_div';
            calendar.style.display = "none";
        } else {
            div.className = 'hidden_div';
            calendar.style.display = "";
        }
    } else if (this.id === 'month_button') {
        div = document.getElementById('month_div');

        div_range.month.x_min = x;
        div_range.month.x_max = x + width;
        div_range.month.y_min = y;
        div_range.month.y_max = y + height;

        if (div.className === 'hidden_div') {
            div.className = 'visible_div';
            document.getElementById('year_div').className = 'hidden_div';
            calendar.style.display = "none";
        } else {
            div.className = 'hidden_div';
            calendar.style.display = "";
        }
    } else {
        return;
    }

    // if (div.className === 'hidden_div') {
    //     div.className = 'visible_div';
    //     calendar.style.display = "none";
    // } else {
    //     div.className = 'hidden_div';
    //     calendar.style.display = "";
    // }

    div.style.left = x + 'px';
    div.style.top = y + height + 'px';
    update_year_month_ui();
    //updateUi();
}

function update_right_pane(year, month, day) {
    var month_stuff = LunarCalendar.calendar(year, month, true, 1);

    var general_datetime_list = document.getElementById('general_datetime_list');
    var datetime_container = document.getElementById('datetime_container');
    var highlight_index = month_stuff['firstDay'] + day - 1;
    var lunar_month_name = month_stuff['monthData'][highlight_index]['lunarMonthName'];
    var lunar_day_name = month_stuff['monthData'][highlight_index]['lunarDayName'];
    var ganzhi_year = month_stuff['monthData'][highlight_index]['GanZhiYear'];
    var ganzhi_month = month_stuff['monthData'][highlight_index]['GanZhiMonth'];
    var ganzhi_day = month_stuff['monthData'][highlight_index]['GanZhiDay'];
    var zodiac = month_stuff['monthData'][highlight_index]['zodiac'];
    var weekday = weekdays[highlight_index % 7];

    console.log(highlight_index);

    var month_str = month.toString();
    if (month <= 9) {
        month_str = '0' + month;
    }
    var day_str = day.toString();
    if (day <= 9) {
        day_str = '0' + day;
    }
    if(NeedChangeCurrentTime)
    {
        datetime_container.children[1].innerHTML = year + '-' + month_str + '-' + day_str + ' ' + weekday /* + ' '+lunar_month_name + lunar_day_name*/;
        NeedChangeCurrentTime = 0;
    }
   /* general_datetime_list.children[0].innerHTML = year + '-' + month_str + '-' + day_str + ' 星期' + weekday;
    general_datetime_list.children[1].innerHTML = day_str; // e.g. 06
    general_datetime_list.children[2].innerHTML = lunar_month_name + lunar_day_name;*/
    //general_datetime_list.children[0].innerHTML = ganzhi_year + '年' + '【' + zodiac + '年' + '】' + ganzhi_month + '月 ' + ganzhi_day + '日';
   // general_datetime_list.children[1].innerHTML = ganzhi_month + '月 ' + ganzhi_day + '日';
    updateTime();
    update_yiji_area();

    month_stuff = null;
}

/* 节日查找从当月开始往后查找，包括公历节日、农历节日和农历节气，最多只查找到下一年 */
function go_to_holiday () {
    var year = today.getFullYear();
    var month = today.getMonth() + 1;
    var day = 0;

    var month_stuff = LunarCalendar.calendar(year, month, false, 1);
    var found = false;
    var target = this.innerHTML;
    do {
        for (var index = 0; index < month_stuff['monthDays']; index++) {
            if (target.indexOf(month_stuff['monthData'][index]['solarFestival']) >= 0 ||
                target.indexOf(month_stuff['monthData'][index]['lunarFestival']) >= 0 ||
                target.indexOf(month_stuff['monthData'][index]['term']) >= 0) {
                day = index + 1;
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }

        if (month === 12) {
            year++;
            month = 1;
        } else {
            month++;
        }

        month_stuff = LunarCalendar.calendar(year, month, false, 1);
    } while (year - today.getFullYear() <= 1);

    if (!found) {
        return;
    }

    year_selector.value = year;
    month_selector.value = month;
    highlight_day = day;
    create_page(year, month);

    month_stuff = null;
}
