// LunarCalendar.js currently supports this range
var year_range = { 'low': 1891, 'high': 2100 };
var weekdays = [ 'ВС', 'ПН', 'ВТ', 'СР', 'ЧТ', 'ПТ', 'СБ' ];
var calendar = null;
var today = new Date();
var highlight_day = today.getDate(); // 鼠标选中的日期值，范围1-31
var year_selector = null;
var month_selector = null;
var hl_script = null;
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

function update_yiji_area() {
    "use strict";

    var year = parseInt(year_selector.value, 10);
    var month = parseInt(month_selector.value, 10);

    var hl_table = document.getElementById('hl_table');
    var current_row;
    var current_cell;

    if (year !== parseInt(hl_script.id, 10)) {
        //load_hl_script(year);
        return;
    }

    if (typeof HuangLi['y' + year] === 'undefined') {
        for (var row = 1; row < 5; row++) {
            if (hl_table.rows.length === row) {
                current_row = hl_table.insertRow(row);
            } else {
                current_row = hl_table.rows[row];
            }

            for (var column = 0; column < 2; column++) {
                if (current_row.cells.length === column) {
                    current_cell = current_row.insertCell(column);
                } else {
                    current_cell = current_row.cells[column];
                }

                if (row === 1) {
                    current_cell.innerHTML = 'No Data';
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

    var hl_data = [yi_str.split('.'), ji_str.split('.')];

    for (var row = 1; row < 5; row++) {
        if (hl_table.rows.length === row) {
            current_row = hl_table.insertRow(row);
        } else {
            current_row = hl_table.rows[row];
        }

        for (var column = 0; column < 2; column++) {
            if (current_row.cells.length === column) {
                current_cell = current_row.insertCell(column);
            } else {
                current_cell = current_row.cells[column];
            }

            if (hl_data[column][row - 1]) {
                current_cell.innerHTML = hl_data[column][row - 1];
            } else {
                current_cell.innerHTML = '';
            }
        }
    }
}

/*function load_hl_script(year) {
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
    //    update_yiji_area();
    };
    document.body.appendChild(hl_script);
}*/

window.onload = function () {
    "use strict";

    //load_hl_script(today.getFullYear());

    var year_list = document.getElementById('year_list');
    var month_list = document.getElementById('month_list');

    var year = today.getFullYear();
    var month = today.getMonth();

    year_selector = document.getElementById('year_selector');
    year_selector.addEventListener('click', popup_div);

    month_selector = document.getElementById('month_selector');
    month_selector.addEventListener('click', popup_div);

    document.addEventListener('click', function(event) {
        var year_div = document.getElementById('year_div');
        var month_div = document.getElementById('month_div');
        var holiday_div = document.getElementById('holiday_div');
        var x = event.clientX;
        var y = event.clientY;

        if (year_div.className === 'visible_div') {
            if (x > div_range.year.x_max || x < div_range.year.x_min ||
                y > div_range.year.y_max || y < div_range.year.y_min) {
                year_div.className = 'hidden_div';
            }
        }
        if (month_div.className === 'visible_div') {
            if (x > div_range.month.x_max || x < div_range.month.x_min ||
                y > div_range.month.y_max || y < div_range.month.y_min ) {
                month_div.className = 'hidden_div';
            }
        }
  /*      if (holiday_div.className === 'visible_div') {
            if (x > div_range.holiday.x_max || x < div_range.holiday.x_min ||
                y > div_range.holiday.y_max || y < div_range.holiday.y_min ) {
                holiday_div.className = 'hidden_div';
            }
        }*/
    });

    for (var index = year_range['low']; index <= year_range['high']; index++) {
        var li = document.createElement('LI');
        li.innerHTML = index;
        li.addEventListener('click', new_month_selected); // new year implies new month
        year_list.appendChild(li);
        if (index === year) {
            year_selector.value = index;
        }
    }

    for (var index = 1; index <= 12; index++) {
        var li = document.createElement('LI');
        li.innerHTML = index;
        li.addEventListener('click', new_month_selected);
        month_list.appendChild(li);
        if (index === month + 1) {
            month_selector.value = index;
        }
    }

    var goto_arrows = document.getElementsByTagName('input');
    var n_months = (year_range['high'] - year_range['low'] + 1) * 12;
    for (var index = 0; index < goto_arrows.length; index++) {
        goto_arrows[index].addEventListener('click', function() {
            var year = parseInt(year_selector.value);
            var month = parseInt(month_selector.value);
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

            create_page(parseInt(year_selector.value), parseInt(month_selector.value));
        });
    }

/*    var holidays = ['元旦节', '春节', '清明节', '劳动节', '端午节', '中秋节', '国庆节'];
    var holiday_list = document.getElementById('holiday_list');
    for (var index = 0; index < holidays.length; index++) {
        var li = document.createElement('LI');
        li.innerHTML = holidays[index];
        li.addEventListener('click', go_to_holiday);
        holiday_list.appendChild(li);
    }*/
 //   var holiday_button = document.getElementById('holiday_button');
 //   holiday_button.addEventListener('click', popup_div);

    var today_button = document.getElementById('today_button');
    today_button.addEventListener('click', function() {
        year_selector.value = today.getFullYear();
        month_selector.value = today.getMonth() + 1;
        highlight_day = today.getDate();
        create_page(today.getFullYear(), today.getMonth() + 1);
    });

    calendar = document.getElementById('calendar_table');
    create_page(parseInt(year_selector.value), parseInt(month_selector.value));
}

function create_page(year, month) {
    if (year < year_range['low'] || year > year_range['high'])
        return;

    var month_stuff = LunarCalendar.calendar(year, month, true);

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
            if (index < month_stuff['firstDay'] || index >= month_stuff['firstDay'] + month_stuff['monthDays']) {
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
            //    worktime = document.createElement("SPAN");
                worktime.className = 'worktime2';
//                worktime.innerHTML = '休';
            } else if (month_stuff['monthData'][index]['worktime'] === 1) {
              //  worktime = document.createElement("SPAN");
                worktime.className = 'worktime1';
//                worktime.innerHTML = '班';
            } else {

            }

            current_cell.innerHTML = '<span class="solar_part">' +
                                     month_stuff['monthData'][index]['day'] +
                                     '</span>' +
                                     '<br />' +
                                     '<span class="lunar_part">' +
                                     '</span>';
            if (worktime && current_cell.className !== 'day_other_month') {
            //    current_cell.appendChild(worktime);
            }
        }
    }

    update_right_pane(year, month, highlight_day);

    month_stuff = null;
}

function new_month_selected() {
    if (this.parentNode.id === 'year_list') {
        year_selector.value = this.innerHTML;
        document.getElementById('year_div').className = 'hidden_div';
    } else if (this.parentNode.id === 'month_list') {
        month_selector.value = this.innerHTML;
        document.getElementById('month_div').className = 'hidden_div';
    }

    create_page(parseInt(year_selector.value), parseInt(month_selector.value));
}

function popup_div(event) {
    var x = event.clientX - event.offsetX;
    var y = event.clientY - event.offsetY;
    var div;

    // TODO
    var width = 64;
    var height = 20;
    if (this.id === 'year_selector') {
        div = document.getElementById('year_div');

        div_range.year.x_min = x;
        div_range.year.x_max = x + width;
        div_range.year.y_min = y;
        div_range.year.y_max = y + height;
    } else if (this.id === 'month_selector') {
        div = document.getElementById('month_div');

        div_range.month.x_min = x;
        div_range.month.x_max = x + width;
        div_range.month.y_min = y;
        div_range.month.y_max = y + height;
    }
    /* else if (this.id === 'holiday_button') {
        div = document.getElementById('holiday_div');
        div.style.width = '64px';
        div.style.height = '100px';

        div_range.holiday.x_min = x;
        div_range.holiday.x_max = x + width;
        div_range.holiday.y_min = y;
        div_range.holiday.y_max = y + height;
    }*/else {
        return;
    }

    if (div.className === 'hidden_div') {
        div.className = 'visible_div';
    } else {
        div.className = 'hidden_div';
    }

    div.style.left = x + 'px';
    div.style.top = y + height + 'px';
}

function update_right_pane(year, month, day) {
    var month_stuff = LunarCalendar.calendar(year, month, true);

    var general_datetime_list = document.getElementById('general_datetime_list');
    var highlight_index = month_stuff['firstDay'] + day - 1;
    var lunar_month_name = month_stuff['monthData'][highlight_index]['lunarMonthName'];
    var lunar_day_name = month_stuff['monthData'][highlight_index]['lunarDayName'];
    var ganzhi_year = month_stuff['monthData'][highlight_index]['GanZhiYear'];
    var ganzhi_month = month_stuff['monthData'][highlight_index]['GanZhiMonth'];
    var ganzhi_day = month_stuff['monthData'][highlight_index]['GanZhiDay'];
    var zodiac = month_stuff['monthData'][highlight_index]['zodiac'];
    var weekday = weekdays[highlight_index % 7];

    var month_str = month.toString();
    if (month <= 9) {
        month_str = '0' + month;
    }
    var day_str = day.toString();
    if (day <= 9) {
        day_str = '0' + day;
    }

    general_datetime_list.children[0].innerHTML = month_str + '-' + day_str + '-' + year + ' ' +  weekday;
    general_datetime_list.children[1].innerHTML = day_str; // e.g. 06
    general_datetime_list.children[2].innerHTML = ' ';
    general_datetime_list.children[3].innerHTML = ' ';
    general_datetime_list.children[4].innerHTML = ' ';

    //update_yiji_area();

    month_stuff = null;
}

/* 节日查找从当月开始往后查找，包括公历节日、农历节日和农历节气，最多只查找到下一年 */
/*function go_to_holiday () {
    var year = today.getFullYear();
    var month = today.getMonth() + 1;
    var day = 0;

    var month_stuff = LunarCalendar.calendar(year, month, false);
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

        month_stuff = LunarCalendar.calendar(year, month, false);
    } while (year - today.getFullYear() <= 1);

    if (!found) {
        return;
    }

    year_selector.value = year;
    month_selector.value = month + 'Month';
    highlight_day = day;
    create_page(year, month);

    month_stuff = null;
}*/
