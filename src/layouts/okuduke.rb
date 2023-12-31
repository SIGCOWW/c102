require 'date'

module ReVIEW
  module PDFMakerOkuduke
    def make_colophon
      super
      t = Time.now
      t.localtime("+09:00")

      rows = []
      if not(@config.key?('history')) || (@config.key?('history') && @config['history'] == 1 && @config['history'][0].length <= 1)
        rows << ['発行日', Date.parse(@config["date"]).strftime("%Y年%-m月%-d日")]
      else
        dates = []
        @config['history'].each_with_index do | val, idx |
          d = Date.parse(val[0])
          dates << [d.strftime("%Y年"), d.strftime("%-m月"), d.strftime("%-d日"), "　第#{idx+1}版 第1刷"]
        end
        tmp = @config['history'].last
        if tmp.length > 1
          d = Date.parse(tmp.last)
          dates << [d.strftime("%Y年"), d.strftime("%-m月"), d.strftime("%-d日"), "　第#{tmp.length}刷"]
        end

        macro_def = ''
        (0..3).each do | i |
          macro_def += "\\newlength{\\strw#{(i+97).chr}}\\settowidth{\\strw#{(i+97).chr}}{}\\newlength{\\tmpstrw#{(i+97).chr}}"
        end

        macro_calc = ''
        dates.each do | v |
          (0..3).each do | i |
            macro_calc += "\\settowidth{\\tmpstrw#{(i+97).chr}}{#{v[i]}}"
            macro_calc += "\\ifdim\\tmpstrw#{(i+97).chr}>\\strw#{(i+97).chr}\\setlength{\\strw#{(i+97).chr}}{\\tmpstrw#{(i+97).chr}}\\fi"
          end
        end

        dates.each_with_index do | val, idx |
          ymd = [*0..3].map{|i| "\\hbox to \\strw#{(i+97).chr}{{\\hfill}#{val[i]}}"}.join('~')
          rows << [idx == 0 ? '発行日' : '', nil, "#{idx == 0 ? macro_def : ''}#{macro_calc}#{ymd}"]
        end
      end

      if @config.key?('pbl')
        rows << ['発　行', @config["pbl"].to_s]
        rows << ['',       @config["oth"].to_s] if @config.key?('oth')
      end
      rows << ['連絡先', @config["edt"].to_s] if @config.key?('edt')
      rows << ['', @config["feedback"].to_s] if @config.key?('feedback')
      rows << ['印刷所', @config["prt"].to_s] if (@config.key?('prt') && not(ENV['ONESIDE']))
      rows << ['ビルド', t.to_s]
      #rows << ['', `review-pdfmaker --version`]
      #rows << ['', `uplatex --version | head -n1`]
      #rows << ['', "Alpine Linux #{`cat /etc/alpine-release`.strip} (Kernel #{`uname -r`.strip})"]
      #rows << ['', "https://github.com/SIGCOWW/desk (#{`cat /etc/desk-release`.strip})"]

      if @config.key?('download') and not(ENV['ONESIDE'])
        list = '12345678abcdefghkmnoprstuvwxyzABCDEFGHJKLMNPRSTUVWXYZ'.split('')
        code = @config["download"].to_s % (0..rand(12..16)).map{list.sample}.join('')
        system("cuterb #{code} cover.png qr.png")
        rows << ['電子版', code]
        rows << ['', nil, '\hfill\includegraphics[width=0.5\textwidth]{../qr.png}\hfill']
      end

      ret = ''
      rows.each do | r |
        key = r[0].empty? ? '' : "\\rule{0pt}{3ex}\\underline{\\mathstrut\\bfseries #{r[0]}}"
        value = r[1].nil? ? r[2] : "#{escape_latex(r[1])}"
        ret += "#{key} & #{value} \\\\\n"
      end
      return ret.strip
    end
  end
  PDFMaker.send(:prepend, PDFMakerOkuduke) if defined? PDFMaker
end
